#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mysql.h>

namespace HY {

	enum polarization
	{
		HH,  // ˮƽ-ˮƽ
		HV   // ˮƽ-��ֱ
	};

	enum thrustState
	{
		MIL,  // ����
		AB   // ����
	};


	struct RCSRecord {
		std::string aircraft_name;
		double frequency;      // GHz
		std::string polarization; // "HH" or "VV"
		double azimuth;        // deg
		double elevation;      // deg
		double rcs_value;   // dBsm

		RCSRecord(std::string name, double freq, std::string pol,
			double az, double el, double rcs)
			: aircraft_name(std::move(name)), frequency(freq),
			polarization(std::move(pol)), azimuth(az),
			elevation(el), rcs_value(rcs) {
		}
	};

	struct IRRecord {
		std::string aircraft_name;  // �ɻ��ͺ�
		std::string thrust_state;   // ����״̬: "MIL" �� "AB"
		int env_temperature;        // �����¶ȣ���λ: ��C����Χ -20 ~ 40
		double azimuth;              // ��λ��: -180 �� 180 ��
		double elevation;            // ������: -90 �� 90 ��
		double ir_value;            // ��������ֵ����λ: W/sr �� dB

		IRRecord(const std::string& name,
			const std::string& thrust,
			int temperature,
			double az,
			double el,
			double ir)
			: aircraft_name(name),
			thrust_state(thrust),
			env_temperature(temperature),
			azimuth(az),
			elevation(el),
			ir_value(ir) {}
	};

	struct ESMRecord {
		std::string aircraft_name;
		std::string system_type;
		std::string mode_name;

		double power;          // ���书�� (W)
		double frequency;      // ����Ƶ�� (GHz)
		double bandwidth;      // ���� (MHz)
		double pulse_width;    // ���� (��s)
		double pri;            // �����ظ���� (��s)
		int beam_number;       // �������
		double minAz;          // ��С��λ�� (��)
		double maxAz;          // ���λ�� (��)
		double minEle;         // ��С������ (��)
		double maxEle;         // ������� (��)
		double max_range;      // ���ʶ����� (km)

		ESMRecord(const std::string& name,
			const std::string& system,
			const std::string& mode,
			double pwr,
			double freq,
			double bw,
			double pw,
			double pri_val,
			int beam,
			double min_az,
			double max_az,
			double min_el,
			double max_el,
			double range)
			: aircraft_name(name),
			system_type(system),
			mode_name(mode),
			power(pwr),
			frequency(freq),
			bandwidth(bw),
			pulse_width(pw),
			pri(pri_val),
			beam_number(beam),
			minAz(min_az),
			maxAz(max_az),
			minEle(min_el),
			maxEle(max_el),
			max_range(range) {}
	};

	struct ECMRecord {
		std::string aircraft_name;
		std::string system_type;
		std::string mode_name;

		double power;          // ���书�� (W)
		double frequency;      // ����Ƶ�� (GHz)
		double bandwidth;      // ���� (MHz)
		double pulse_width;    // ���� (��s)
		double pri;            // �����ظ���� (��s)
		int beam_number;       // �������
		double minAz;          // ��С��λ�� (��)
		double maxAz;          // ���λ�� (��)
		double minEle;         // ��С������ (��)
		double maxEle;         // ������� (��)
		double max_range;      // ���ʶ����� (km)

		ECMRecord(const std::string& name,
			const std::string& system,
			const std::string& mode,
			double pwr,
			double freq,
			double bw,
			double pw,
			double pri_val,
			int beam,
			double min_az,
			double max_az,
			double min_el,
			double max_el,
			double range)
			: aircraft_name(name),
			system_type(system),
			mode_name(mode),
			power(pwr),
			frequency(freq),
			bandwidth(bw),
			pulse_width(pw),
			pri(pri_val),
			beam_number(beam),
			minAz(min_az),
			maxAz(max_az),
			minEle(min_el),
			maxEle(max_el),
			max_range(range) {}
	};

	class RCSCache {
	private:
		std::vector<RCSRecord> records;
		std::vector<IRRecord> ir_records;
		std::vector<ESMRecord> esm_records;
		std::vector<ECMRecord> ecm_records;
	public:

		std::unordered_map<std::string, std::vector<HY::RCSRecord>> name_index;
		std::unordered_map<std::string, std::vector<HY::IRRecord>> name_index_ir;
		std::unordered_map<std::string, std::vector<HY::ESMRecord>> name_index_esm;
		std::unordered_map<std::string, std::vector<HY::ECMRecord>> name_index_ecm;

		bool loadFromDB(const std::string& host, const std::string& user, const std::string& password, const std::string& database);


		std::vector<HY::RCSRecord> findByAircraft(const std::string& name);


	};






}