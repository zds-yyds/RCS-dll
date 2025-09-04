#pragma once
#include "HYSimulation_export.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "DBCache.h"
#include "license.h"

using namespace HY;


struct POSITION {
	double lon;
	double lat;
	double alt;
	double heading;
	double pitch;
	double roll;
};

struct GeoCoord {
	double lat; // 纬度，度
	double lon; // 经度，度
	double alt; // 高度，米
};

struct FOV {
	double minAz, maxAz;   // deg
	double minEle, maxEle; // deg
	double maxRange;       // km
};



class HYSimulation_API Signature {
public:
	Signature(RCSCache path)
		: cache(path)
	{
	}
	
	double GetRCS(POSITION Pos, double maxRange/*km*/, double minAz, double maxAz, double minEle, double maxEle, double freq, polarization pol, std::string platformName, POSITION myPos);

	double GetRCS(POSITION Pos, double freq, polarization pol, std::string platformName, POSITION myPos);

	double GetIR(POSITION Pos, double temperature, thrustState thrust, std::string platformName, POSITION myPos);

	std::vector<ESMRecord> GetESM(POSITION Pos, std::string platformName, POSITION myPos);

	std::vector<ECMRecord> GetECM(POSITION Pos, std::string platformName, POSITION myPos);

	/// 将经纬高转换为 ECEF 坐标
	void LLA2ECEF(const GeoCoord& geo, double& x, double& y, double& z) {
		double lat = geo.lat * DEG2RAD;
		double lon = geo.lon * DEG2RAD;
		double R = EARTH_RADIUS + geo.alt;
		x = R * cos(lat) * cos(lon);
		y = R * cos(lat) * sin(lon);
		z = R * sin(lat);
	}

	/// 将目标点转换到观测方 ENU 坐标系
	void ECEF2ENU(const GeoCoord& obs, double tx, double ty, double tz, double& ex, double& ey, double& ez) {
		double lat = obs.lat * DEG2RAD;
		double lon = obs.lon * DEG2RAD;

		double ox, oy, oz;
		LLA2ECEF(obs, ox, oy, oz);

		double dx = tx - ox;
		double dy = ty - oy;
		double dz = tz - oz;

		// ENU 矩阵
		double t[3][3] = {
			{-sin(lon),           cos(lon),          0},
			{-sin(lat) * cos(lon), -sin(lat) * sin(lon), cos(lat)},
			{ cos(lat) * cos(lon),  cos(lat) * sin(lon), sin(lat)}
		};

		ex = t[0][0] * dx + t[0][1] * dy + t[0][2] * dz;
		ey = t[1][0] * dx + t[1][1] * dy + t[1][2] * dz;
		ez = t[2][0] * dx + t[2][1] * dy + t[2][2] * dz;
	}

	/// 判断是否在视场
	bool InFOV(const GeoCoord& obs, double heading_deg, double pitch_deg, const FOV& fov, const GeoCoord& tgt)
	{
		// 1. 坐标转换
		double tx, ty, tz;
		LLA2ECEF(tgt, tx, ty, tz);

		double ex, ey, ez;
		ECEF2ENU(obs, tx, ty, tz, ex, ey, ez);

		double range = sqrt(ex * ex + ey * ey + ez * ez);
		if (range > fov.maxRange) return false;

		// 2. 转换为水平坐标系角度
		double az = atan2(ex, ey) / DEG2RAD;    // 注意：ENU下 az = atan2(East, North)
		double el = atan2(ez, sqrt(ex * ex + ey * ey)) / DEG2RAD;

		// 3. 考虑观测方的 heading、pitch
		az -= heading_deg;
		el -= pitch_deg;

		// 将角度归一化到 [-180, 180]
		auto normDeg = [](double a) {
			while (a > 180) a -= 360;
			while (a < -180) a += 360;
			return a;
		};
		az = normDeg(az);
		el = normDeg(el);

		// 4. 判断是否在视场范围
		if (az < fov.minAz || az > fov.maxAz) return false;
		if (el < fov.minEle || el > fov.maxEle) return false;

		return true;
	}

	/// 判断目标是否在观测者视线上的可见区域（不被地球挡住）
	bool IsVisible(const GeoCoord& obs, const GeoCoord& tgt) {
		// 1. 获取观测者和目标的 ECEF 坐标
		double ox, oy, oz;
		double tx, ty, tz;
		LLA2ECEF(obs, ox, oy, oz);
		LLA2ECEF(tgt, tx, ty, tz);

		// 2. 构建从观测者指向目标的方向向量
		double dx = tx - ox;
		double dy = ty - oy;
		double dz = tz - oz;

		// 3. 标准化方向向量
		double dNorm = sqrt(dx*dx + dy*dy + dz*dz);
		dx /= dNorm;
		dy /= dNorm;
		dz /= dNorm;

		// 4. 射线起点到地心的向量
		double oxv = ox;
		double oyv = oy;
		double ozv = oz;

		// 5. 计算射线是否与地球球体（地心，半径 R）相交
		double b = 2 * (oxv*dx + oyv*dy + ozv*dz);
		double c = oxv*oxv + oyv*oyv + ozv*ozv - EARTH_RADIUS * EARTH_RADIUS;

		double discriminant = b*b - 4 * c;

		if (discriminant < 0) {
			// 没有交点，视线没穿过地球 ⇒ 可见
			return true;
		}

		// 有交点，计算最近的交点距离
		double t1 = (-b - sqrt(discriminant)) / 2.0;
		double t2 = (-b + sqrt(discriminant)) / 2.0;

		// 判断交点是否在目标之前
		double rangeToTarget = dNorm;
		if (t1 > 1e-6 && t1 < rangeToTarget) {
			// 目标之前有地球挡住 ⇒ 不可见
			return false;
		}

		return true;
	}

	///在视场内且视线上的可见
	bool InFOVAndVisible(const GeoCoord& obs, double heading_deg, double pitch_deg, const FOV& fov, const GeoCoord& tgt) {
		bool one = InFOV(obs, heading_deg, pitch_deg, fov, tgt);
		bool two = IsVisible(obs, tgt);
		return one && two;
	}

	/// 从被观测者角度，计算观测者的相对方位角和俯仰角
	void RelativeAzEl(const GeoCoord& observer, const GeoCoord& target, double heading_deg, double pitch_deg, double& relAz_deg, double& relEl_deg);

	/// 从本地缓存中查找对应的RCS值
	double findRCS(std::vector<RCSRecord>& records, const std::string& name, double frequency, const std::string& polarization, double azimuth, double elevation);

	/// 从本地缓存中查找对应的IR值
	double findIR(std::vector<IRRecord>& records, const std::string& name, const std::string& thrust_state, int env_temperature, double azimuth, double elevation);

	bool loadFromDB(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
	std::vector<HY::RCSRecord> findByAircraft(const std::string& name);

	RCSCache cache;
private:
	// RCS
	double current_RCS = 0.0;

	double current_IR = 0.0;

	double current_ESM = 0.0;

	double current_ECM = 0.0;

	const double PI = 3.14159265358979323846;
	const double DEG2RAD = PI / 180.0;
	const double RAD2DEG = 180.0 / PI;
	const double EARTH_RADIUS = 6371000.0; // m

	//std::vector<RCSRecord> records_RCS; // RCS
	//std::vector<RCSRecord> records; // IR
	//std::vector<RCSRecord> records; // ESM
	//std::vector<RCSRecord> records; // ECM

};



