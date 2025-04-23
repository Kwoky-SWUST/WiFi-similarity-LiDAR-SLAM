	//添加处
	int max_RSS_num = 0;
	for (long int current_time = min_timestamp; current_time <= max_timestamp; current_time = current_time + timeWindow)
	{
		long int timestamp = current_time;
		m_radio_fingerprints_extended[timestamp].sensor_integrated = 0; // 先把此刻 WiFi序列 map中 的 sensor_integrated 赋为0
		std::map<std::string, int>::iterator it_scan;
		std::map<long int, MobileScanning>::iterator iterator_search; // first-timestamp second-MobileScanning 里面有wifi_scan

		// for wifi scan
		std::map<std::string, int> wifi_scan;				 // first-mac地址 second-mac对应的RSS
		std::map<std::string, std::vector<int>> v_wifi_scan; // first-mac地址 这段时间内second-mac对应的RSS的数组

		for (iterator_search = m_radio_fingerprints.begin(); iterator_search != m_radio_fingerprints.end(); ++iterator_search) // 每次都遍历所有的WiFi指纹 根据timestamp来切割
		{
			int time_diff = fabs(iterator_search->first - timestamp); // 以此时的 timestamp 向前和向后截取 m_extend_time_threshold 时间戳长度的WiFi
			if (time_diff <= m_extend_time_threshold)
			{
				// we store the cell id and rss 应该是mac地址和RSS吧
				std::map<std::string, int> temp_wifi_scan = iterator_search->second.wifi_scan; // 把此刻的wifi_scan存储到temp_wifi_scan中

				for (it_scan = temp_wifi_scan.begin(); it_scan != temp_wifi_scan.end(); ++it_scan)
				{
					v_wifi_scan[it_scan->first].push_back(it_scan->second); // 把这个时间段所有 对应mac的RSS存进去
				}
			}
		}
		if (max_RSS_num < v_wifi_scan.size())
		{
			max_RSS_num = v_wifi_scan.size();
		}

		std::map<std::string, std::vector<int>>::iterator it_wifi_scan_vector; // 迭代器
		for (it_wifi_scan_vector = v_wifi_scan.begin(); it_wifi_scan_vector != v_wifi_scan.end(); ++it_wifi_scan_vector)
		{
			std::vector<int> v_rss = it_wifi_scan_vector->second; // 定义一个数组 来存储 迭代器当前代表的 mac下的 RSS数组
			double mean_rss = 0;
			double count = 0;
			double sum_rss = 0;
			for (int i = 0; i < v_rss.size(); i++)
			{
				sum_rss = sum_rss + v_rss[i];
				count = count + 1;
			}
			mean_rss = sum_rss / count;																   // 求平均： 对这个mac地址下的所有 RSS 求和 并除以 RSS 数组的大小
			m_radio_fingerprints_extended[timestamp].wifi_scan[it_wifi_scan_vector->first] = mean_rss; //然后把这个平均值记录在 当前时间戳 WiFi序列的MobileScanning 下 当前mac 地址的RSS中
		}

		// for cell scan
		double measurement_in_duration = 0;
		std::map<std::string, int> cell_scan;
		std::map<std::string, std::vector<int>> v_cell_scan;

		for (iterator_search = m_radio_fingerprints.begin(); iterator_search != m_radio_fingerprints.end(); ++iterator_search) //便利整个指纹内容 把时间符合的一小段截出来
		{
			int time_diff = fabs(iterator_search->first - timestamp);
			if (time_diff <= m_extend_time_threshold)
			{
				// we store the cell id and rss
				std::map<std::string, int> temp_cell_scan = iterator_search->second.cell_scan;
				for (it_scan = temp_cell_scan.begin(); it_scan != temp_cell_scan.end(); ++it_scan)
				{
					v_cell_scan[it_scan->first].push_back(it_scan->second);
				}

				measurement_in_duration = measurement_in_duration + 1; // 记录在一个WiFi序列的长度中 有几个WiFi指纹
			}
		}

		std::map<std::string, std::vector<int>>::iterator it_cell_scan_vector;
		for (it_cell_scan_vector = v_cell_scan.begin(); it_cell_scan_vector != v_cell_scan.end(); ++it_cell_scan_vector)
		{
			std::vector<int> v_rss = it_cell_scan_vector->second;
			double mean_rss = 0;
			double count = 0;
			double sum_rss = 0;
			for (int i = 0; i < v_rss.size(); i++)
			{
				sum_rss = sum_rss + v_rss[i];
				count = count + 1;
			}
			mean_rss = sum_rss / count;

			double detection_prob = count / measurement_in_duration; // 这个WiFi序列中 平均每个WiFi指纹的cell id地址数量

			m_radio_fingerprints_extended[timestamp].cell_scan[it_cell_scan_vector->first] = mean_rss;
		}
	}