#include "DBCache.h"

bool HY::RCSCache::loadFromDB(const std::string & host, const std::string & user, const std::string & password, const std::string & database)
{

	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_RES* res_ir;
	MYSQL_RES* res_esm;
	MYSQL_RES* res_ecm;
	MYSQL_ROW row;
	MYSQL_ROW row_ir;
	MYSQL_ROW row_esm;
	MYSQL_ROW row_ecm;

	// 初始化
	conn = mysql_init(nullptr);
	if (conn == nullptr) {
		std::cerr << "mysql_init() failed\n";
		return false;
	}

	// 连接数据库
	if (mysql_real_connect(conn,
		host.c_str(),   // 主机
		user.c_str(),        // 用户
		password.c_str(),      // 密码
		database.c_str(),// 数据库
		3306,          // 端口
		nullptr,       // Unix socket/管道
		0) == nullptr) // flags
	{
		std::cerr << "mysql_real_connect() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	// 执行查询
	if (mysql_query(conn, "SELECT * FROM rcs_data")) {
		std::cerr << "SELECT failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}
	res = mysql_store_result(conn);
	if (res == nullptr) {
		std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	if (mysql_query(conn, "SELECT * FROM ir_data")) {
		std::cerr << "SELECT failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}
	res_ir = mysql_store_result(conn);
	if (res_ir == nullptr) {
		std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	if (mysql_query(conn, "SELECT * FROM esm_data")) {
		std::cerr << "SELECT failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}
	res_esm = mysql_store_result(conn);
	if (res_esm == nullptr) {
		std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	if (mysql_query(conn, "SELECT * FROM ecm_data")) {
		std::cerr << "SELECT failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}
	res_ecm = mysql_store_result(conn);
	if (res_ecm == nullptr) {
		std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	

	// 输出结果
	int num_fields = mysql_num_fields(res);
	MYSQL_FIELD* fields = mysql_fetch_fields(res);

	while ((row = mysql_fetch_row(res))) {

		int id = row[0] ? std::stoi(row[0]) : 0;
		std::string name = row[1] ? row[1] : "NULL";
		double freq = row[2] ? std::stod(row[2]) : 0.0;
		std::string pol = row[3] ? row[3] : "NULL";
		double az = row[4] ? std::stod(row[4]) : 0.0;
		double el = row[5] ? std::stod(row[5]) : 0.0;
		double rcs = row[6] ? std::stod(row[6]) : 0.0;

		records.emplace_back(name, freq, pol, az, el, rcs);

		const HY::RCSRecord rec = records.back();
		name_index[name].push_back(rec);
	}

	while ((row_ir = mysql_fetch_row(res_ir))) {
		int id = row_ir[0] ? std::stoi(row_ir[0]) : 0;
		std::string name = row_ir[1] ? row_ir[1] : "NULL";
		std::string thrust_state = row_ir[2] ? row_ir[2] : "NULL";
		int env_temp = row_ir[3] ? std::stoi(row_ir[3]) : 0;
		double azimuth = row_ir[4] ? std::stod(row_ir[4]) : 0.0;
		double elevation = row_ir[5] ? std::stod(row_ir[5]) : 0.0;
		double ir_value = row_ir[6] ? std::stod(row_ir[6]) : 0.0;

		ir_records.emplace_back(name, thrust_state, env_temp, azimuth, elevation, ir_value);

		const IRRecord& rec = ir_records.back();
		name_index_ir[name].push_back(rec);
	}

	while ((row_esm = mysql_fetch_row(res_esm))) {
		int id = row_esm[0] ? std::stoi(row_esm[0]) : 0;
		std::string name = row_esm[1] ? row_esm[1] : "NULL";
		std::string system_type = row_esm[2] ? row_esm[2] : "NULL";
		std::string mode_name = row_esm[3] ? row_esm[3] : "NULL";

		double power = row_esm[4] ? std::stod(row_esm[4]) : NAN;
		double frequency = row_esm[5] ? std::stod(row_esm[5]) : NAN;
		double bandwidth = row_esm[6] ? std::stod(row_esm[6]) : NAN;
		double pulse_width = row_esm[7] ? std::stod(row_esm[7]) : NAN;
		double pri = row_esm[8] ? std::stod(row_esm[8]) : NAN;
		int beam_number = row_esm[9] ? std::stoi(row_esm[9]) : -1;
		double minAz = row_esm[10] ? std::stod(row_esm[10]) : NAN;
		double maxAz = row_esm[11] ? std::stod(row_esm[11]) : NAN;
		double minEle = row_esm[12] ? std::stod(row_esm[12]) : NAN;
		double maxEle = row_esm[13] ? std::stod(row_esm[13]) : NAN;
		double max_range = row_esm[14] ? std::stod(row_esm[14]) : NAN;

		esm_records.emplace_back(
			name, system_type, mode_name,
			power, frequency, bandwidth,
			pulse_width, pri, beam_number,
			minAz, maxAz, minEle, maxEle, max_range
			);

		const ESMRecord& rec = esm_records.back();
		name_index_esm[name].push_back(rec);
	}

	while ((row_ecm = mysql_fetch_row(res_ecm))) {
		std::string name = row_ecm[1] ? row_ecm[1] : "NULL";
		std::string system_type = row_ecm[2] ? row_ecm[2] : "NULL";
		std::string mode_name = row_ecm[3] ? row_ecm[3] : "NULL";

		double power = row_ecm[4] ? std::stod(row_ecm[4]) : NAN;
		double frequency = row_ecm[5] ? std::stod(row_ecm[5]) : NAN;
		double bandwidth = row_ecm[6] ? std::stod(row_ecm[6]) : NAN;
		double pulse_width = row_ecm[7] ? std::stod(row_ecm[7]) : NAN;
		double pri = row_ecm[8] ? std::stod(row_ecm[8]) : NAN;
		int beam_number = row_ecm[9] ? std::stoi(row_ecm[9]) : -1;
		double minAz = row_ecm[10] ? std::stod(row_ecm[10]) : NAN;
		double maxAz = row_ecm[11] ? std::stod(row_ecm[11]) : NAN;
		double minEle = row_ecm[12] ? std::stod(row_ecm[12]) : NAN;
		double maxEle = row_ecm[13] ? std::stod(row_ecm[13]) : NAN;
		double max_range = row_ecm[14] ? std::stod(row_ecm[14]) : NAN;

		ecm_records.emplace_back(
			name, system_type, mode_name,
			power, frequency, bandwidth,
			pulse_width, pri, beam_number,
			minAz, maxAz, minEle, maxEle, max_range
			);

		const ECMRecord& rec = ecm_records.back();
		name_index_ecm[name].push_back(rec);
	}

	// 释放资源
	mysql_free_result(res);
	mysql_free_result(res_ir);
	mysql_free_result(res_esm);
	mysql_free_result(res_ecm);
	mysql_close(conn);
	return true;
}


std::vector<HY::RCSRecord> HY::RCSCache::findByAircraft(const std::string& name)
{
	static std::vector<RCSRecord> empty; //

	auto it = name_index.find(name);
	if (it != name_index.end()) {
		return it->second;
	}
	return empty;
}

