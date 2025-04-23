#include <stdlib.h>
#include <cmath>

#include <g2o/core/sparse_optimizer.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/factory.h>
#include <g2o/core/optimization_algorithm_factory.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/solvers/csparse/linear_solver_csparse.h>
#include <g2o/types/slam2d/parameter_se2_offset.h>
#include <g2o/core/optimizable_graph.h>
#include <g2o/types/slam2d/vertex_se2.h>
#include <g2o/types/slam2d/edge_se2.h>
#include <g2o/types/slam3d/vertex_se3.h>
#include <g2o/types/slam3d/edge_se3.h>
#include <g2o/types/slam3d/parameter_se3_offset.h>
#include <g2o/types/slam3d/vertex_pointxyz.h>
#include <g2o/types/slam3d/edge_se3_pointxyz_distance.h>
#include <g2o/types/slam3d/edge_se3_distance.h>

#include <carmen/carmen.h>
#include <carmenx/ambisense_slam/ambisense_slam_interface.h>
#include <xstream/z.h>
#include <xstream/bz.h>
#include <gzstream/gzstream.h>
#include <set>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <utils/gnuplot/gnuplot.h>
#include <utils/text/font_style.h>
#include <utils/text/to_string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <time.h>
#include <stdio.h>
#include <conversion.h>

#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>

using namespace GeographicLib;
using namespace std;
using namespace g2o;

/** Information of a mobile APP scanning */
// 移动端APP扫描的信息 包括wifi_scan，frequency，xyz，累计距离，几个标记
class MobileScanning
{
public:
	MobileScanning()
	{
	}
	std::map<std::string, int> cell_scan; // cell id and the RSS
	std::map<std::string, int> wifi_scan; // wifi mac address and rss
	std::map<std::string, int> frequency; // wifi frequency

	double longitude; // GPS coordinates:longitude
	double latitude;  // GPS coordinates:latitude
	double x;		  // the x
	double y;		  // the y
	double z;		  // the z

	double accumulated_dist; //累计行驶距离

	double odom_x;
	double odom_y;
	double odom_z;

	int index_robot;
	// 这里是用于整合 里程计 和 WiFi指纹的 标记位 内容是m_robot_odometry中的first参数 也就是时间戳 表示本wifi_scan中最匹配的 RobotOdom 是时间戳为index_robot的RobotOdom
	int active_earfcn;
	// ###indicates the active earfcn in a scan 这个也好像没啥用
	int sensor_integrated;
	// ###是否有信息融合的标记位	这个好像没啥用啊
	int gt_found;
	// 是否在m_robot_ground_truth中找到对应的groundtruth的标记位
};

/** Recording the robot odometry */
// 记录机器人的里程计 里程计的时间是按秒计的 包括xyz，roll pitch yaw，qx,qy,qz,qw，时间戳，累计距离
class RobotOdom
{
public:
	RobotOdom()
	{
	}
	double x;
	double y;
	double z;
	double roll;
	double pitch;
	double yaw;
	double qx;		  // 四元数
	double qy;		  //
	double qz;		  //
	double qw;		  //
	double timestamp; // time stamp is in double and in seconds
	double accumulated_dist;
};

std::map<int, RobotOdom> m_robot_odometry; // raw robot odometry 机器人里程计的构造 first是时间戳 以s为单位

std::map<long int, MobileScanning> m_radio_fingerprints; // recording the radio fingerprints 记录指纹 ###-first 为长整型用来记录时间戳？ 以ms为单位

std::map<long int, MobileScanning> m_radio_fingerprints_extended; // WiFi序列

std::map<int, RobotOdom> m_robot_odometry_optimized; // record the optimized odometry 记录优化算法后的里程计 ###利用WiFi约束来优化里程计？
													 // -first 为整形，记录时间，s为单位 second为RobotOdom  （包括xyz，roll pitch yaw，qx,qy,qz,qw，时间戳，累计距离）

std::map<int, RobotOdom> m_robot_ground_truth; // robot ground truth we get from AMCL    groundtruth是从激光来的 -first是从0开始依次往上增长的整形

int m_record_count_ground_truth = 0; // number of lines recording the ground truth 		gt的行数

std::map<long int, MobileScanning> m_radio_fingerprints_optimized; // record the optimized results记录优化的WiFi指纹序列

int number_records = 0;		//这是啥

std::map<string, int> m_mac_rss;

// parameters for the grid representation of the fingerprint map
// 指纹栅格地图的参数
double m_min_grid_x = -200;		// min x
double m_max_grid_x = 200;		// max x
double m_min_grid_y = -100;		// min y
double m_max_grid_y = 300;		// max y
double m_grid_resolution = 5.0; // resolution of the grid

class GridFingerprintRepresentation //指纹地图的表达 ：包括wifi_scan cell_scan x y z	这东西有点类似m_radio_fingerprints
{
public:
	GridFingerprintRepresentation()
	{
	}
	std::map<std::string, int> cell_scan; // cell id and the RSS
	std::map<std::string, int> wifi_scan; // wifi mac address and rss
	double x;							  // the x
	double y;							  // the y
	double z;							  // the z
};

class FingerprintPair 				//指纹对 其中的f1 f2 是匹配的两个WiFi序列的 时间戳
{
public:
	FingerprintPair()
	{
	}
	long int f1;
	long int f2;
};

std::map<int, GridFingerprintRepresentation> m_grid_fingerprint_map; 	// fingerprint map 指纹地图 -first为int second为GridFingerprintRepresentation

int m_record_count = 0; 												// number of nodes in the graph 图优化的点数量

int m_odom_record; 						// number of odom nodes in the graph, anything higher this number will be regarded as the landmark 
										// 图优化中里程计的点的数量，高于这个数值的就会被视为地标  地标是WiFi指纹?

std::string m_username;

char *m_fingerprints_file_name;	 // record fingerprints file name 指纹的文件名
char *m_robot_odom_file_name;	 // record robot odo file name 里程计文件名
char *m_robot_ground_truth_file; // the robot ground truth file path   AMCL激光的文件名

double m_sim_threshold = 0.7; //最小的相似度阈值 0.7只是做一个初始化

double min_cell_verify_rss = INFINITY; //最小的信号强度  在读取WiFi的时候会计算WiFi中的信号强度最小值

std::istringstream iss_line_wifi; //“istringstream::istringstream(string str)”的作用是从string对象str中读取字符。
								  // istringstream对象可以绑定一行字符串，然后以空格为分隔符把该行分隔开。

// gnuplot
GnuplotInterface *m_plot_samples = new GnuplotInterface();
GnuplotInterface *m_plot_alligned = new GnuplotInterface();
GnuplotInterface *m_plot_similarity_model = new GnuplotInterface();

std::map<int, g2o::SE3Quat> m_raw_se3quat_pose;				  // vertex--->pose, before optimization 			优化之前：把顶点转换成位姿 ###first是什么	map类型
std::map<int, g2o::SE3Quat> m_optimized_se3quat_pose;		  // vertex--->pose, after optimization 			优化之后：把顶点转成位姿
std::vector<carmen_6d_point_t> m_v_landmark_pose;			  // optimized landmark (access point) pose 		优化的地标位姿
std::map<std::string, carmen_6d_point_t> m_landmark_pose_map; // optimized landmark position, string->pose 	优化的地标位姿 另一种表达方式
std::map<std::string, int> m_mac_counter;					  // 如果该mac地址的 rss frequency 满足筛选调剂 则 m_mac_counter[mac_address]的int 标志位就会被置为1
															  // 后续用m_mac_counter.size()方法就可以求有效信号的mac数量了 ！！mac数量不会被重新计算
std::map<std::string, int> m_lte_counter;

/** Sample of a distance-->similarity measure */
//由距离 计算 相似度
class SimilartyDistance
{
public:
	SimilartyDistance()
	{
	}
	double similarity; //相似度
	double distance;   //距离
	double delta_x;	   // x差值
	double delta_y;	   // y差值
	double delta_z;	   // z差值
};

/** similarty-->variance */
//由相似度 计算 方差
class SimilartyVariance
{
public:
	SimilartyVariance()
	{
	}
	double similarity; //相似度
	double variance;   //方差
	double mean;	   //###平均值 是什么的平均值
};

std::vector<SimilartyDistance> m_similarty_distance_wifi; //一个数组来保存SimilartyDistance 来作为WiFi序列之间的相似度 保存了任意两个WiFi序列之间的相似度	 //应该是把相似度和距离的映射进行保存
std::vector<SimilartyVariance> m_similarty_variance_wifi; //根据相似度范围[min_sim,max_sim] 计算相似度在范围内时 对应的坐标距离之差 的方差					//把相似度和方差的映射进行保存
std::vector<SimilartyDistance> m_similarty_distance_lte;
std::vector<SimilartyVariance> m_similarty_variance_lte;

// 画图函数 传入的是一个方差的vector
// x坐标是相似度[0~1] y坐标是距离 表示相似度和距离直接的相关性
void plot_similarity_model(std::vector<SimilartyVariance> similarty_variance)
{

	string cmd; //( "set size ratio 1\n");
	cmd += "set grid\n";
	cmd += "set xlabel 'similarity'\n";
	cmd += "set ylabel 'distance'\n";
	cmd += "set xrange [0:1]\n";
	// cmd+="set yrange [-80:60]\n";
	cmd += "set size ratio 0.5\n";

	cmd += "plot '-' u 1:2 w lp pt 2 lw 1 lt 2 lc 3 ti 'Similarity model',\n";

	for (int i = 0; i < similarty_variance.size(); i++)
	{
		cmd += toString(similarty_variance[i].similarity) + ' ' + toString(similarty_variance[i].mean) + ' ' + toString(0) + '\n';
		//	std::cout<<"raw:"<<pose_se3quat.translation()[0]<<" "<<pose_se3quat.translation()[1]<<" "<<pose_se3quat.translation()[2]<<std::endl;
	}

	cmd += "e\n";

	m_plot_similarity_model->commandStr(cmd);
}

// 输出轨迹图的函数 传入两个map参数 std::map<int, g2o::SE3Quat> raw_pose和std::map<int, g2o::SE3Quat> optimized_pose
// x y 坐标单位都是 m 范围是 x[-25~45] y[-20~130] 没有使用这个
void plot_track(std::map<int, g2o::SE3Quat> raw_pose, std::map<int, g2o::SE3Quat> optimized_pose)
{

	string cmd; //( "set size ratio 1\n");
	cmd += "set grid\n";
	cmd += "set xlabel 'x(m)'\n";
	cmd += "set ylabel 'y(m)'\n";
	cmd += "set xrange [-25:45]\n";
	cmd += "set yrange [-20:130]\n";

	std::map<int, g2o::SE3Quat>::iterator it_;

	cmd += "plot '-' u 1:2 w lp pt 2 lw 1 lt 2 lc 3 ti 'raw track','-' u 1:2 w lp pt 6 lw 1 lt 1 lc 1 ti 'optimized track',\n";

	for (it_ = raw_pose.begin(); it_ != raw_pose.end(); ++it_)
	{

		g2o::SE3Quat pose_se3quat = it_->second;
		cmd += toString(pose_se3quat.translation()[0]) + ' ' + toString(pose_se3quat.translation()[1]) + ' ' + toString(pose_se3quat.translation()[2]) + '\n';
		// ###pose_se3quat.translation()[0]是什么作用？
		// std::cout<<"odo:"<<pose_se3quat.translation()[0]<<" "<<pose_se3quat.translation()[1]<<std::endl;
	}

	cmd += "e\n";

	for (it_ = optimized_pose.begin(); it_ != optimized_pose.end(); ++it_)
	{

		g2o::SE3Quat pose_se3quat = it_->second;
		cmd += toString(pose_se3quat.translation()[0]) + ' ' + toString(pose_se3quat.translation()[1]) + ' ' + toString(pose_se3quat.translation()[2]) + '\n';
		// std::cout<<"opt:"<<pose_se3quat.translation()[0]<<" "<<pose_se3quat.translation()[1]<<std::endl;
	}

	cmd += "e\n";

	m_plot_samples->commandStr(cmd);
}

// 画直线轨迹？？ 传入两个参数std::vector<carmen_point_t> ground_truth ，std::vector<carmen_point_t> estimation ###carmen_point_t是什么东西
void plot_track_alignment(std::vector<carmen_point_t> ground_truth, std::vector<carmen_point_t> estimation)
{

	string cmd; //( "set size ratio 1\n");
	cmd += "set grid\n";
	cmd += "set xlabel 'x(m)'\n";
	cmd += "set ylabel 'y(m)'\n";
	cmd += "set xrange [-30:45]\n";
	cmd += "set yrange [-80:60]\n";
	cmd += "set size ratio -1\n";

	std::map<int, g2o::SE3Quat>::iterator it_;

	cmd += "plot '-' u 1:2 w p pt 2 lw 1 lt 2 lc 3 ti 'Ground Truth','-' u 1:2 w p pt 6 lw 1 lt 1 lc 1 ti 'Optimized Track',\n";

	for (int i = 0; i < ground_truth.size(); i++)
	{

		cmd += toString(ground_truth[i].x) + ' ' + toString(ground_truth[i].y) + ' ' + toString(0) + '\n';
		// std::cout<<"raw:"<<ground_truth[i].x<<" "<<ground_truth[i].y<<std::endl;
	}

	cmd += "e\n";

	for (int i = 0; i < estimation.size(); i++)
	{

		cmd += toString(estimation[i].x) + ' ' + toString(estimation[i].y) + ' ' + toString(0) + '\n';
		//	std::cout<<"optimized:"<<estimation[i].x<<" "<<estimation[i].y<<std::endl;
	}

	cmd += "e\n";

	m_plot_alligned->commandStr(cmd);
}

// 画没有地标的轨迹地图 和plot_track传入的参数是一样的 ### 目前没有发现这个函数和plot_track有啥区别
void plot_trackWithoutLandmark(std::map<int, g2o::SE3Quat> raw_pose, std::map<int, g2o::SE3Quat> optimized_pose)
{

	string cmd; //( "set size ratio 1\n");
	cmd += "set grid\n";
	cmd += "set xlabel 'x(m)'\n";
	cmd += "set ylabel 'y(m)'\n";
	cmd += "set xrange [-25:45]\n";
	cmd += "set yrange [-20:130]\n";
	cmd += "set size ratio -1\n";

	std::map<int, g2o::SE3Quat>::iterator it_;

	cmd += "plot '-' u 1:2 w lp pt 2 lw 1 lt 2 lc 3 ti 'raw track','-' u 1:2 w lp pt 6 lw 1 lt 1 lc 1 ti 'optimized track',\n";

	for (it_ = raw_pose.begin(); it_ != raw_pose.end(); ++it_)
	{

		g2o::SE3Quat pose_se3quat = it_->second;
		cmd += toString(pose_se3quat.translation()[0]) + ' ' + toString(pose_se3quat.translation()[1]) + ' ' + toString(pose_se3quat.translation()[2]) + '\n';
		//	std::cout<<"raw:"<<pose_se3quat.translation()[0]<<" "<<pose_se3quat.translation()[1]<<" "<<pose_se3quat.translation()[2]<<std::endl;
	}

	cmd += "e\n";

	for (it_ = optimized_pose.begin(); it_ != optimized_pose.end(); ++it_)
	{

		g2o::SE3Quat pose_se3quat = it_->second;
		cmd += toString(pose_se3quat.translation()[0]) + ' ' + toString(pose_se3quat.translation()[1]) + ' ' + toString(pose_se3quat.translation()[2]) + '\n';
		// std::cout<<"optimized:"<<pose_se3quat.translation()[0]<<" "<<pose_se3quat.translation()[1]<<" "<<pose_se3quat.translation()[2]<<std::endl;
	}

	cmd += "e\n";

	m_plot_samples->commandStr(cmd);
}

// 如果遇到数据读取错误 传入参数为 MYSQL *con
void finish_with_error(MYSQL *con)
{
	fprintf(stderr, "%s\n", mysql_error(con)); // fprintf？？ mysql_error？？ mysql_close？
	mysql_close(con);
	exit(1);
}

// 从command line读取参数 传入参数是main 函数的传入参数int argc, char **argv 在函数里修改m_fingerprints_file_name、m_robot_odom_file_name、m_robot_ground_truth_file、m_sim_threshold
int read_parameters_from_command_line(int argc, char **argv)
{
	carmen_read_commandline_parameters(argc, argv); // carmen_read_commandline_parameters这是啥

	if (carmen_find_param("fingerprints") && carmen_find_param_pair("fingerprints"))
	{
		m_fingerprints_file_name = carmen_process_param_string("fingerprints", NULL);
	}
	else
	{
		std::cout << "Please specify the fingerprints file name using -fingerprints" << std::endl;
		exit(0);
	}

	if (carmen_find_param("robotOdom") && carmen_find_param_pair("robotOdom"))
	{
		m_robot_odom_file_name = carmen_process_param_string("robotOdom", NULL);
	}
	else
	{
		std::cout << "Please specify the verification file name using -robotOdom" << std::endl;
		exit(0);
	}

	if (carmen_find_param("groundTruth") && carmen_find_param_pair("groundTruth"))
	{
		m_robot_ground_truth_file = carmen_process_param_string("groundTruth", NULL);
	}

	if (carmen_find_param("simThreshold") && carmen_find_param_pair("simThreshold"))
	{
		char *param = carmen_process_param_string("simThreshold", NULL);

		m_sim_threshold = strtod(param, NULL);
	}
	else
	{
		std::cout << "Please specify the threshold parameter  using -simThreshold" << std::endl;
		exit(0);
	}

	return 0;
}

// 希罗克 生成基于时间的随机数 ###不会用这个
void sciroco_generate_time_based_random_seed()
{
	unsigned int seed;
	struct timeval tv;

	if (gettimeofday(&tv, NULL) < 0)
		fprintf(stderr, "error in get time of day : %s\n", strerror(errno));
	seed = tv.tv_sec + tv.tv_usec;
	srand(seed);
}

// convert the RSS to distance 把RSS信息转换成距离信息
double getDistance(double rss, double PL0, double eta)
{
	// parameter power law of RSS to distance, reference to formular:rss(d)=P_T-PL(d0)-10nlog10(d/d0)+X_sigma
	double power_factor = (PL0 - rss) / (10.0 * eta); //这是RSS到 距离 的公式 ###pow是什么

	double distance = pow(10.0, power_factor);
	return distance;
}

// 把RSS强度从-100到-30 都利用getDistance转化成距离 并画图画出来
void rssModel()
{
	string cmd; //( "set size ratio 1\n"); 设置大小比1
	cmd += "set grid\n";
	cmd += "set xlabel 'x(m)'\n";
	cmd += "set ylabel 'y(m)'\n";
	cmd += "set xrange [0:100]\n";
	cmd += "set yrange [-100:-30]\n";
	cmd += "set size ratio -1\n";
	cmd += "plot '-' u 1:2 w lp pt 2 lw 2 lt 2 lc 3 ti 'rss model',\n";

	for (int rss = -100; rss <= -30; rss++)
	{
		std::cout << rss << " " << getDistance(rss, -40, 1.5) << std::endl;
		cmd += toString(getDistance(rss, -45, 2)) + ' ' + toString(rss) + ' ' + toString(0) + '\n';
	}
	cmd += "e\n";

	m_plot_samples->commandStr(cmd);
}

// 加载AMCL得到的位置数据
// 根据文件的扩展名来选择不同的打开方式 有.gz .bz2等
// 把读取的timestamp, x, y, z,整合到一个Odom类里，然后赋值给 m_robot_ground_truth的m_record_count_ground_truth(first参数)下 m_record_count_ground_truth是从0开始增长的整形
void loadGroundTruth(char *model_name)
{
	std::string modelFileName(model_name);
	std::string line, key_word;
	std::istringstream iss_line;

	std::ifstream model_in(modelFileName.c_str(), std::ios::in); // iostream 处理控制台 IO；fstream 处理命名文件 IO；stringstream 完成内存 string 的IO

	if (!model_in)
	{
		std::cerr << "Sensor model file " << modelFileName << " not found, please re-check it." << std::endl;
		exit(0);
	}

	xstream::bz::istreambuf *bz_ibuf = NULL;
	std::istream *fin = NULL; // istream的用法学习一下 std::istream->good？

	// Open log file as gzipped/bzipped/raw ASCII stream depending on its extension
	if (modelFileName.find(".gz") == modelFileName.length() - 3)
	{
		model_in.close();
		fin = new igzstream(modelFileName.c_str());
		if (!fin)
		{
			std::cerr << "Error opening gzipped file:" << modelFileName << std::endl;
			exit(0);
		}
	}
	else if (modelFileName.find(".bz2") == modelFileName.length() - 4)
	{
		bz_ibuf = new xstream::bz::istreambuf(model_in.rdbuf());
		fin = new std::istream(bz_ibuf);
		if (!fin)
		{
			std::cerr << "Error opening bzipped file:" << modelFileName << std::endl;
			exit(0);
		}
		fin->exceptions(std::ios::badbit);
	}
	else
	{
		fin = new std::istream(model_in.rdbuf());
		if (!fin)
		{
			std::cerr << "Error opening regular file:" << modelFileName << std::endl;
			exit(0);
		}
	}

	// load key parameters
	size_t key_word_separator; // 关键词分隔符
	uint line_count = 0;	   // 行数计数
	uint total_line_count = 0; // 总函数计数 ###这俩有啥区别

	while (fin->good())
	{
		iss_line.clear();
		getline(*fin, line); //读取fin流中的字符 赋值给line。 getline函数可读取整行，包括前导和嵌入的空格
							 // getline用法：getline(cin, inputLine); 其中 cin 是正在读取的输入流，而 inputLine 是接收输入字符串的 string 变量的名称。 getline可以客服cin输入方式会被空格等分隔符影响的问题

		total_line_count++;

		// Filter out comment lines 过滤掉注释和空白行
		if (line.length() == 0 || line[0] == '#')
			continue;

		// Now, all remaining lines have a key word in their beginning, followed by white spaces and the rest of the data
		// 现在，所有剩余的行的开头都有一个关键字，后面是空格和其他数据

		key_word_separator = line.find(' ');		 // find('') 如果找到' '，则返回该子字符串首次出现时其首字符的索引；否则，返回string::npos
		if (key_word_separator == std::string::npos) // std::string::npos的意思： npos可以表示string的结束位置 find函数在找不到指定值得情况下会返回string::npos
			continue;								 //如果发现分隔符已经是该行的最后一个字符了，那就不要再继续下面的操作了，直接跳出当前的while循环

		iss_line.str(line); //把line的内容存入到iss_line字符串流中

		RobotOdom odom;												 //构建里程计对象odom
		double timestamp, x, y, z, roll, pitch, yaw, qx, qy, qz, qw; //感觉roll, pitch, yaw, qx, qy, qz, qw这些好像没用啊
		long int timestamp_long;
		long int timestamp_head_long;
		string frame; //定义字符串对象frame
		int frame_id; //每个帧有个对应的帧id？

		// istringstream 字符串流可以根据 空格分隔符   将不同的分开的 字符串 转换成 其他类型 的变量
		iss_line >> timestamp_long;
		iss_line >> frame_id;
		iss_line >> timestamp_head_long;
		iss_line >> frame;

		iss_line >> x;
		iss_line >> y;
		iss_line >> z;

		timestamp = timestamp_long / 1000000000.0;

		odom.timestamp = timestamp;
		odom.x = x;
		odom.y = y;
		odom.z = z;

		m_robot_ground_truth[m_record_count_ground_truth] = odom; // m_record_count_ground_truth是从0开始计数的一个整形

		m_record_count_ground_truth = m_record_count_ground_truth + 1;
	}
}

// 加载机器人里程计信息
// 从文件中读取后 把timestamp, x, y, z, roll, pitch, yaw, qx, qy, qz, qw全部赋给m_robot_odometry[m_record_count] 其中m_record_count是从0开始的一个整形
void loadRobotOdom(char *model_name)
{
	std::string modelFileName(model_name);
	std::string line, key_word;
	std::istringstream iss_line;

	std::ifstream model_in(modelFileName.c_str(), std::ios::in);

	if (!model_in)
	{
		std::cerr << "Sensor model file " << modelFileName << " not found, please re-check it." << std::endl;
		exit(0);
	}

	xstream::bz::istreambuf *bz_ibuf = NULL;
	std::istream *fin = NULL;

	// Open log file as gzipped/bzipped/raw ASCII stream depending on its extension
	if (modelFileName.find(".gz") == modelFileName.length() - 3)
	{
		model_in.close();
		fin = new igzstream(modelFileName.c_str());
		if (!fin)
		{
			std::cerr << "Error opening gzipped file:" << modelFileName << std::endl;
			exit(0);
		}
	}
	else if (modelFileName.find(".bz2") == modelFileName.length() - 4)
	{
		bz_ibuf = new xstream::bz::istreambuf(model_in.rdbuf());
		fin = new std::istream(bz_ibuf);
		if (!fin)
		{
			std::cerr << "Error opening bzipped file:" << modelFileName << std::endl;
			exit(0);
		}
		fin->exceptions(std::ios::badbit); //清空当前输入流状态，设置状态为badbit，当流的状态变成所设置的状态的时候，就会抛出一个异常，也就是当后续输入流中出现badbit状态时，抛出一个错误  也就是检测所读取内容是否完整
	}
	else
	{
		fin = new std::istream(model_in.rdbuf());
		if (!fin)
		{
			std::cerr << "Error opening regular file:" << modelFileName << std::endl;
			exit(0);
		}
	}

	// load key parameters
	size_t key_word_separator; // size_t 类型表示C++中任何对象所能达到的最大长度，它是无符号整数

	int total_line_count = 0;

	while (fin->good())
	{
		iss_line.clear();
		getline(*fin, line);

		// Filter out comment lines
		if (line.length() == 0 || line[0] == '#')
			continue;

		// Now, all remaining lines have a key word in their beginning,
		// followed by white spaces and the rest of the data
		key_word_separator = line.find(' ');
		if (key_word_separator == std::string::npos)
			continue;

		iss_line.str(line);

		RobotOdom odom;
		double timestamp, x, y, z, roll, pitch, yaw, qx, qy, qz, qw;

		iss_line >> timestamp;
		iss_line >> x;
		iss_line >> y;
		iss_line >> z;
		iss_line >> roll;
		iss_line >> pitch;
		iss_line >> yaw;
		iss_line >> qx;
		iss_line >> qy;
		iss_line >> qz;
		iss_line >> qw;

		odom.timestamp = timestamp;
		odom.x = x;
		odom.y = y;
		odom.z = z;
		odom.roll = roll;
		odom.pitch = pitch;
		odom.yaw = yaw;

		odom.qx = qx;
		odom.qy = qy;
		odom.qz = qz;
		odom.qw = qw;

		// std::cout<<odom.timestamp<<" "<<odom.x<<" "<<odom.y<<" "<<odom.z<<" "<<odom.yaw<<std::endl;
		// printf("%.10f\n", odom.timestamp);

		// if(total_line_count%10==0)
		{
			m_robot_odometry[m_record_count] = odom;
			m_record_count = m_record_count + 1;
		}

		total_line_count = total_line_count + 1;
	}
}

void loadFingerprints(char *model_name)
{
	std::string modelFileName(model_name);
	std::string line, key_word; //这个key_word好像没啥用啊...
	std::istringstream iss_line;

	std::ifstream model_in(modelFileName.c_str(), std::ios::in);

	if (!model_in)
	{
		std::cerr << "Sensor model file " << modelFileName << " not found, please re-check it." << std::endl;
		exit(0);
	}

	xstream::bz::istreambuf *bz_ibuf = NULL;
	std::istream *fin = NULL;

	// Open log file as gzipped/bzipped/raw ASCII stream depending on its extension
	if (modelFileName.find(".gz") == modelFileName.length() - 3)
	{
		model_in.close();
		fin = new igzstream(modelFileName.c_str());
		if (!fin)
		{
			std::cerr << "Error opening gzipped file:" << modelFileName << std::endl;
			exit(0);
		}
	}
	else if (modelFileName.find(".bz2") == modelFileName.length() - 4)
	{
		bz_ibuf = new xstream::bz::istreambuf(model_in.rdbuf());
		fin = new std::istream(bz_ibuf);
		if (!fin)
		{
			std::cerr << "Error opening bzipped file:" << modelFileName << std::endl;
			exit(0);
		}
		fin->exceptions(std::ios::badbit);
	}
	else
	{
		fin = new std::istream(model_in.rdbuf());
		if (!fin)
		{
			std::cerr << "Error opening regular file:" << modelFileName << std::endl;
			exit(0);
		}
	}

	// load key parameters
	size_t key_word_separator;
	uint line_count = 0;
	uint total_line_count = 0;

	while (fin->good())
	{
		iss_line.clear();
		getline(*fin, line);

		total_line_count++;
		// Filter out comment lines
		if (line.length() == 0 || line[0] == '#')
			continue;

		// Now, all remaining lines have a key word in their beginning,
		// followed by white spaces and the rest of the data
		key_word_separator = line.find(' ');
		if (key_word_separator == std::string::npos)
			continue;

		iss_line.str(line);

		// we extract the content of the measurements
		long int timestamp;
		std::map<std::string, int> wifi_scan;	   // wifi mac address and rss
		std::map<std::string, int> wifi_frequency; // wifi mac address and frequency
		int mcc, mnc, eci, ucid, band, dBm, tac, earfcn, uarfcn, rsrp, rsrq, rssnr, cqi, ta, lac, cid, num_wifi_mac;
		string registered;
		string imei;
		string psc;
		string pci;
		string unique_cell_id;
		double lat, lon, accuracy, ori_x, ori_y, ori_z;

		// time,MCC,MNC,ECI,UCID,Band,dBm,isRegistered,PCI,TAC,earfcn,uarfcn,rsrp,rsrq,rssnr,cqi,ta,lac,PSC,Lat,Lon,WIFI

		iss_line >> imei; //国际移动设备识别码（International Mobile Equipment Identity，IMEI），即通常所说的手机序列号、手机“串号”，用于在移动电话网络中识别每一部独立的手机等移动通信设备，相当于移动电话的身份证。
		iss_line >> timestamp;
		iss_line >> mcc;  // MCC 是 Mobile Country Code 的缩写，译为移动国家代码。它由三位数字组成
		iss_line >> mnc;  // MNC 是 Mobile Network Code 的缩写，译为移动网络代码。它由二到三位数字组成。  和 MCC 合在一起唯一标识一个移动网络提供者。
		iss_line >> eci;  // ECI（E-UTRAN Cell Identifier，E-UTRAN 小区唯一标识）：主要起移动用户定位唯一标识作用。
		iss_line >> ucid; // Universal Call Identifier
		iss_line >> band;
		iss_line >> dBm; // dbm是一个表示功率绝对值。dBm越大，表示周围信号越好，如-76dbm>-100dbm。
		iss_line >> registered;
		iss_line >> pci;	// PCI（物理小区标识）
		iss_line >> tac;	// TAC（Tracking Area Code，跟踪区代码）：由运营商自行分配，主要起移动用户定位唯一标识作用。
		iss_line >> earfcn; // EARFCN：载波频点号（E-UTRA Absolute Radio Frequency Channel Number），绝对无线频率信道号，使用16bit表示，范围是0-65535
		iss_line >> uarfcn; // UTRA absolute radio frequency channel number 绝对无线频率信道号
		iss_line >> rsrp;	// RSRP：参考信号接收功率，是LTE小区网络中接收功率电平的测量值，平均功率是从单个参考信号接收的功率的测量值，值越大信号越好。
		iss_line >> rsrq;	// RSRQ（ReferenceSignalReceivingQuality）：表示LTE参考信号接收质量，这种度量主要是根据信号质量来对不同LTE候选小区进行排序。
		iss_line >> rssnr;	// RS SNR 信噪比
		iss_line >> cqi;	// CQI是信道质量的信息指示，代表当前信道质量的好坏，和信道的信噪比大小相对应，取值范围0～31
		iss_line >> ta;		// 不知道是啥
		iss_line >> lac;	// LAC，Location Area Code，位置区域码			CID，Cell Identity，基站编号，是个16位的数据
		iss_line >> psc;	// PSC是一种本地小区标识符 它是本地"唯一的
		iss_line >> lat;	// latitude
		iss_line >> lon;	// Longitude
		iss_line >> accuracy;
		iss_line >> ori_x;
		iss_line >> ori_y;
		iss_line >> ori_z;
		iss_line >> num_wifi_mac; // 后面还会根据当前收到的WiFi数量 把每个 mac下的rss存到 wifi_scan下 同时 这个这个mac下的频率会被存到wifi_frequency下

		// if(timestamp>=1575370440000)
		// continue;

		if (dBm < min_cell_verify_rss)
		{
			min_cell_verify_rss = dBm; //这个好像没啥用
		}

		if (uarfcn != 0)
		{
			unique_cell_id = "psc_" + psc + "_" + toString(uarfcn) + "_" + toString(band);
		}
		else
		{
			if (earfcn != 0)
			{
				unique_cell_id = "pci_" + pci + "_" + toString(earfcn) + "_" + toString(band);
			}
			else
			{
				unique_cell_id = "not_valid";
			}
		}

		for (int i = 0; i < num_wifi_mac; i++)
		{
			string mac_address;
			int frequency;
			int rss;
			iss_line >> mac_address;
			iss_line >> frequency;
			iss_line >> rss;

			wifi_scan[mac_address] = rss;
			wifi_frequency[mac_address] = frequency;
		}

		if (band > 0)
		{
			if (uarfcn != 0)
			{
				if (dBm < 0)
				{
					// m_radio_fingerprints[timestamp].cell_scan[unique_cell_id]=dBm;
				}
			}
			else
			{
				if (dBm < 0)
				//	if(1)
				{
					if (earfcn == 3250 || earfcn == 3725 || earfcn == 1300 || earfcn == 1444 || earfcn == 3050 || earfcn == 1850)
					// if(earfcn==3250||earfcn==3725||earfcn==3050||earfcn==1850)
					// if(earfcn==3050||earfcn==1850)
					{
						m_radio_fingerprints[timestamp].cell_scan[unique_cell_id] = dBm;
						// m_radio_fingerprints[timestamp].wifi_scan[unique_cell_id]=dBm;
						m_lte_counter[unique_cell_id] = m_lte_counter[unique_cell_id] + 1;
					}
					/*
					if(registered=="true")
					{
						//m_ref_map_mobile_scan[timestamp].active_earfcn=;
						//std::cout<<timestamp<<" "<<mcc<<" "<<mnc<<" "<<pci<<" "<<dBm<<" "<<earfcn<<" "<<registered<<" "<<m_radio_fingerprints[timestamp].active_earfcn<<std::endl;
						m_radio_fingerprints[timestamp].active_earfcn=earfcn;
						m_radio_fingerprints[timestamp].cell_scan[unique_cell_id]=dBm;

					}
					else
					{
						//we find the scan for the specific timestamp
						std::map< long int, MobileScanning >::iterator it_mobile_scan;

						it_mobile_scan=m_radio_fingerprints.find(timestamp);

						if(it_mobile_scan!=m_radio_fingerprints.end())
						{
							int active_earfcn=it_mobile_scan->second.active_earfcn;
							if(earfcn==active_earfcn)
							{
								m_radio_fingerprints[timestamp].cell_scan[unique_cell_id]=dBm;

							}
						}

					}*/
				}
			}
		}

		//把从文件里读取到的 经度纬度 传给对应的timestamp的m_radio_fingerprints 以及给对应的timestamp的sensor_integrated赋值为0
		m_radio_fingerprints[timestamp].longitude = lon;
		m_radio_fingerprints[timestamp].latitude = lat;
		m_radio_fingerprints[timestamp].sensor_integrated = 0;

		number_records = number_records + 1;

		std::map<std::string, int>::iterator iterator_wifi_scan; // iterator

		for (iterator_wifi_scan = wifi_scan.begin(); iterator_wifi_scan != wifi_scan.end(); ++iterator_wifi_scan)
		{
			int min_rss = -100;

			if (iterator_wifi_scan->second > min_rss)
				if (iterator_wifi_scan->first != "d0:c6:37:09:62:3a") //这d0:c6:37:09:62:3a是啥意思呢
				// if(wifi_frequency[iterator_wifi_scan->first]>5000)
				{
					// 如果满足上述条件 则赋值 对应的时间戳的MobileScanning中的wifi_scan(first-string_mac second-int_rss)对应的mac下的rss
					m_radio_fingerprints[timestamp].wifi_scan[iterator_wifi_scan->first] = iterator_wifi_scan->second;
					// std::cout<<iterator_wifi_scan->first<<" "<<iterator_wifi_scan->second<<std::endl;

					// frequency(first-string_mac second-int_frequency)
					m_radio_fingerprints[timestamp].frequency[iterator_wifi_scan->first] = wifi_frequency[iterator_wifi_scan->first]; //###这里是不是要改成second？？？

					// 如果这个mac下的rss 和frequency满足条件 被记录之后 就把m_mac_counter对应mac地址的标志位改成1
					m_mac_counter[iterator_wifi_scan->first] = m_mac_counter[iterator_wifi_scan->first] + 1;
				}
		}
	}
}

// 计算余弦相似度 用cross 除以根号下 sum_f1，sum_f2的积  sum_f1，sum_f2是对RSS求平方和
double getCosineSimilarity(std::map<std::string, int> f1, std::map<std::string, int> f2)
{
	std::map<string, int>::iterator it_mac_rss;

	// we first compute the sum of the first f1

	double sum_f1 = 0;
	double sum_f2 = 0;
	double count_f1 = 0;
	double count_f2 = 0;
	double cross = 0;

	//对RSS求平方和 对mac地址数求和
	for (it_mac_rss = f1.begin();
		 it_mac_rss != f1.end(); it_mac_rss++)
	{
		sum_f1 = sum_f1 + (it_mac_rss->second) * (it_mac_rss->second);
		count_f1 = count_f1 + 1;
		// std::cout<<it_mac_rss->first<<" "<<it_mac_rss->second<<std::endl;
	}

	for (it_mac_rss = f2.begin();
		 it_mac_rss != f2.end(); it_mac_rss++)
	{
		sum_f2 = sum_f2 + (it_mac_rss->second) * (it_mac_rss->second);
		count_f2 = count_f2 + 1;
	}

	int common_ap = 0; //相同给的mac地址数量
	for (it_mac_rss = f1.begin();
		 it_mac_rss != f1.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			// we find the mac and we should caculate the cross of the value
			// cross = 第一个WiFi指纹下的mac_1对应的RSS * 第二个WiFi指纹下的mac_2对应的RSS
			cross = cross + (it_mac_rss->second) * (it_ref_mac_rss->second);
			common_ap = common_ap + 1;
		}
	}

	//如果没有相同的WiFi路由器 则返回相似度为0 否则计算余弦相似度： 用cross 除以根号下 sum_f1，sum_f2的积
	if (common_ap <= 0)
	{
		return 0.0;
	}
	else
	{
		double cos_similarity = cross / sqrt(sum_f1 * sum_f2);
		return cos_similarity;
	}
}

// 计算两个WiFi指纹中相同的mac地址数的占比 ：(2.0 * common_ap) / (count_f1 + count_f2)
double getCommonDetections(std::map<std::string, int> f1, std::map<std::string, int> f2)
{
	std::map<string, int>::iterator it_mac_rss;

	double sum_f1 = 0;
	double sum_f2 = 0;
	double count_f1 = 0;
	double count_f2 = 0;
	double cross = 0;

	//先对f1 f2 的mac地址数量求和
	for (it_mac_rss = f1.begin(); it_mac_rss != f1.end(); it_mac_rss++)
	{
		count_f1 = count_f1 + 1;
		// std::cout<<it_mac_rss->first<<" "<<it_mac_rss->second<<std::endl;
	}
	for (it_mac_rss = f2.begin();
		 it_mac_rss != f2.end(); it_mac_rss++)
	{
		count_f2 = count_f2 + 1;
	}

	//计算f1 f2中相同的mac地址的数量
	int common_ap = 0;
	for (it_mac_rss = f1.begin();
		 it_mac_rss != f1.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			// we find the mac and we should caculate the cross of the value
			common_ap = common_ap + 1;
		}
	}

	if (common_ap <= 0)
	{
		return 0.0;
	}
	else
	{
		double common_similarity = ((double)(2.0 * common_ap)) / (count_f1 + count_f2);
		return common_similarity;
	}
}

// 把两个WiFi指纹中相同mac地址的RSS 求和之后 存入detections这个vector中 然后把仅存在f1 或仅存在f2中的RSS存到missing_detections对应的mac下
// 以便后面计算 RSS和 两个相似的WiFi序列中 相同mac地址的比例 的模型
void checkMissingDetections(std::map<std::string, int> f1, std::map<std::string, int> f2, std::vector<int> &detections, std::vector<int> &missing_detections)
{
	std::map<string, int>::iterator it_mac_rss;

	for (it_mac_rss = f1.begin(); it_mac_rss != f1.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			int mean_rss = (it_mac_rss->second + it_ref_mac_rss->second) / 2;
			// we find the ap in both f1 and f2
			// detections.push_back(it_mac_rss->second);
			// detections.push_back(it_ref_mac_rss->second);
			detections.push_back(mean_rss);
			// std::cout<<it_mac_rss->second<<" "<<it_ref_mac_rss->second<<std::endl;
		}
		else
		{
			// we find the ap in f1, but not in f2
			missing_detections.push_back(it_mac_rss->second);
		}
	}

	for (it_mac_rss = f2.begin(); it_mac_rss != f2.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f1.find(it_mac_rss->first);
		if (it_ref_mac_rss == f1.end())
		{
			// we find the ap in f2, but not in f1
			missing_detections.push_back(it_mac_rss->second);
		}
	}
}

// 利用高斯公式计算两个WiFi指纹之间的相似度
double getGaussianSimilarity(std::map<std::string, int> f1, std::map<std::string, int> f2)
{
	std::map<string, int>::iterator it_mac_rss;

	int number_cells_f1 = 0;
	int number_cells_f2 = 0;
	double min_likelihood = 0.001;
	double sigma = 6.0;

	double product_likelihood = 1.0;
	double num_common_cells = 0;

	for (it_mac_rss = f1.begin(); it_mac_rss != f1.end(); it_mac_rss++)
	{

		number_cells_f1 = number_cells_f1 + 1;
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			double current_likelihood =
				exp(-0.5 * (it_mac_rss->second - it_ref_mac_rss->second) * (it_mac_rss->second - it_ref_mac_rss->second) / (sigma * sigma));
			// we compute the likelihood given two rssi
			if (current_likelihood <= min_likelihood)
			{
				current_likelihood = min_likelihood;
			}

			product_likelihood = product_likelihood * current_likelihood;
			num_common_cells = num_common_cells + 1;
		}
	}

	//
	for (it_mac_rss = f2.begin(); it_mac_rss != f2.end(); it_mac_rss++)
	{
		number_cells_f2 = number_cells_f2 + 1;
	}

	// std::cout<<cross<<" "<<sum_f1<<" "<<sum_f2<<",common ap:"<<common_ap<<" "<<f1.rss_measure.size()<<" "<<f2.rss_measure.size()<<std::endl;
	if (f1.size() <= 0 || f2.size() <= 0 || num_common_cells <= 0.001)
	{
		return 0.0;
	}
	else
	{
		double powered = pow(product_likelihood, 1.0 / num_common_cells);
		return powered * num_common_cells * 2.0 / (number_cells_f1 + number_cells_f2);
	}
}

// 另一个高斯计算公式 来计算WiFi指纹之间的相似度
double getGaussianSimilarityOriginal(std::map<std::string, int> f1, std::map<std::string, int> f2)
{
	std::map<string, int>::iterator it_mac_rss;
	// we first compute the sum of the first f1

	int number_cells = 0;
	double min_likelihood = 0.001;
	double sum_likelihood = 0;
	double sigma = 6.0;

	for (it_mac_rss = f1.begin();
		 it_mac_rss != f1.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			double current_likelihood =
				exp(-0.5 * (it_mac_rss->second - it_ref_mac_rss->second) * (it_mac_rss->second - it_ref_mac_rss->second) / (sigma * sigma));
			// we compute the likelihood given two rssi
			if (current_likelihood <= min_likelihood)
			{
				current_likelihood = min_likelihood;
			}

			sum_likelihood = sum_likelihood + current_likelihood;
			number_cells = number_cells + 1;
		}
		else
		{
			sum_likelihood = sum_likelihood + min_likelihood;
			number_cells = number_cells + 1;
		}
	}

	for (it_mac_rss = f2.begin();
		 it_mac_rss != f2.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f1.find(it_mac_rss->first);
		if (it_ref_mac_rss == f1.end())
		{
			sum_likelihood = sum_likelihood + min_likelihood;
			// the cell id is not in f1
			number_cells = number_cells + 1;
		}
	}

	// std::cout<<cross<<" "<<sum_f1<<" "<<sum_f2<<",common ap:"<<common_ap<<" "<<f1.rss_measure.size()<<" "<<f2.rss_measure.size()<<std::endl;
	if (f1.size() <= 0 || f2.size() <= 0 || number_cells <= 0)
	{
		return 0.0;
	}
	else
	{
		return sum_likelihood / ((double)(number_cells));
	}
}

double getGaussianSimilarityWithPenalty(std::map<std::string, int> f1, std::map<std::string, int> f2, int min_rss, int mas_rss, std::map<int, double> miss_detection_likelihood)
{
	std::map<string, int>::iterator it_mac_rss;
	// we first compute the sum of the first f1

	int number_cells_f1 = 0;
	int number_cells_f2 = 0;
	double min_likelihood = 0.001;
	double sigma = 6.0;
	double penalty = 1.0;
	double penalty_sigma = 6.0;

	double product_likelihood = 1.0;
	double num_common_cells = 0;

	double num_missing_detection = 0;

	//
	for (it_mac_rss = f1.begin();
		 it_mac_rss != f1.end(); it_mac_rss++)
	{

		number_cells_f1 = number_cells_f1 + 1;
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f2.find(it_mac_rss->first);
		if (it_ref_mac_rss != f2.end())
		{
			double current_likelihood =
				exp(-0.5 * (it_mac_rss->second - it_ref_mac_rss->second) * (it_mac_rss->second - it_ref_mac_rss->second) / (sigma * sigma));
			// we compute the likelihood given two rssi
			if (current_likelihood <= min_likelihood)
			{
				current_likelihood = min_likelihood;
			}

			product_likelihood = product_likelihood * current_likelihood;
			num_common_cells = num_common_cells + 1;
		}
		else
		{
			int rss = it_mac_rss->second;
			if (it_mac_rss->second >= mas_rss)
			{
				rss = mas_rss;
			}
			if (it_mac_rss->second <= min_rss)
			{
				rss = min_rss;
			}

			double penalty_value = miss_detection_likelihood[rss];

			double penalty_likelihood = exp(-0.5 * penalty_value * penalty_value / (penalty_sigma * penalty_sigma));
			penalty = penalty * penalty_likelihood;
			num_missing_detection = num_missing_detection + 1;
		}
	}

	for (it_mac_rss = f2.begin();
		 it_mac_rss != f2.end(); it_mac_rss++)
	{
		std::map<string, int>::iterator it_ref_mac_rss;
		it_ref_mac_rss = f1.find(it_mac_rss->first);
		if (it_ref_mac_rss == f1.end())
		{
			// we find the ap in f2, but not in f1
			int rss = it_mac_rss->second;
			if (it_mac_rss->second >= mas_rss)
			{
				rss = mas_rss;
			}
			if (it_mac_rss->second <= min_rss)
			{
				rss = min_rss;
			}

			double penalty_value = miss_detection_likelihood[rss];

			double penalty_likelihood = exp(-0.5 * penalty_value * penalty_value / (penalty_sigma * penalty_sigma));
			penalty = penalty * penalty_likelihood;
			num_missing_detection = num_missing_detection + 1;
		}
	}

	//
	for (it_mac_rss = f2.begin();
		 it_mac_rss != f2.end(); it_mac_rss++)
	{
		number_cells_f2 = number_cells_f2 + 1;
	}

	// std::cout<<cross<<" "<<sum_f1<<" "<<sum_f2<<",common ap:"<<common_ap<<" "<<f1.rss_measure.size()<<" "<<f2.rss_measure.size()<<std::endl;
	if (f1.size() <= 0 || f2.size() <= 0 || num_common_cells <= 0.001)
	{
		return 0.0;
	}
	else
	{
		if (num_missing_detection <= 0.01)
		{
			double powered = pow(product_likelihood, 1.0 / num_common_cells);
			// return powered*num_common_cells*2.0/(number_cells_f1+number_cells_f2);
			return powered;
			;
		}
		else
		{
			double powered = pow(product_likelihood, 1.0 / num_common_cells);
			// return penalty*powered*num_common_cells*2.0/(number_cells_f1+number_cells_f2);
			return powered;
			;
		}
	}
}

std::multimap<int, string> flip_multi_mapStr(std::map<string, int> &src)
{
	std::multimap<int, string> dst;
	std::multimap<string, int>::iterator it;
	for (it = src.begin(); it != src.end(); ++it)
		dst.insert(std::pair<int, string>(it->second, it->first));
	return dst;
}

int main(int argc, char **argv)
{
	//rssModel();
	//sleep(100);
	sciroco_generate_time_based_random_seed(); // set random (!) random seed 生成一个基于时间的随机数

	read_parameters_from_command_line(argc, argv); //从command line读取参数 传入参数是main 函数的传入参数int argc, char **argv
												   //在函数里修改m_fingerprints_file_name、m_robot_odom_file_name、m_robot_ground_truth_file、m_sim_threshold

	// load the configure file
	loadFingerprints(m_fingerprints_file_name); // load the fingerprints data 加载指纹数据

	loadRobotOdom(m_robot_odom_file_name); // load robot odom加载里程计数据

	loadGroundTruth(m_robot_ground_truth_file); // we need to load the ground truth based on AMCL 从AMCL得到的结果获取gt 也就是位置数据

	std::map<int, RobotOdom>::iterator it_current_odom; //迭代器

	std::map<int, RobotOdom>::iterator it_ref_odom; //迭代器

	std::map<std::string, int>::iterator it_lte_address; //迭代器

	// 这个for好像没用
	// for (it_lte_address = m_lte_counter.begin(); it_lte_address != m_lte_counter.end(); it_lte_address++)
	// {
	// 	// std::cout<<it_lte_address->first<<" "<<it_lte_address->second<<std::endl;
	// }
	// return 1;
	// add the nodes in the graph

	// we calculate the accumulated distacne
	// 计算所有时间的累积距离
	double accumulated_dist = 0;
	for (it_current_odom = m_robot_odometry.begin(); it_current_odom != m_robot_odometry.end(); it_current_odom++)
	{
		if (it_current_odom == m_robot_odometry.begin())
		{
			m_robot_odometry[it_current_odom->first].accumulated_dist = accumulated_dist; //=0
		}
		else
		{
			it_ref_odom = m_robot_odometry.find(it_current_odom->first - 1); //这一秒的x y z - 上一秒的x y z 在求平方和 再开根号 得到这一秒的位移 再对位移求合
			if (it_ref_odom != m_robot_odometry.end())
			{
				// we caculated the distance moved
				double previous_x = it_ref_odom->second.x;
				double previous_y = it_ref_odom->second.y;
				double previous_z = it_ref_odom->second.z;

				double x = it_current_odom->second.x;
				double y = it_current_odom->second.y;
				double z = it_current_odom->second.z;

				double dis_diff = sqrt((x - previous_x) * (x - previous_x) + (y - previous_y) * (y - previous_y) + (z - previous_z) * (z - previous_z));
				accumulated_dist = accumulated_dist + dis_diff;
				m_robot_odometry[it_current_odom->first].accumulated_dist = accumulated_dist;
			}
		}
	}
	std::cout << "累计移动距离为" << accumulated_dist << std::endl;

	// std::map< long int, MobileScanning > m_radio_fingerprints;	//recording the radio fingerprints

	std::map<long int, MobileScanning>::iterator it_current_fingerprint;
	std::map<long int, MobileScanning>::iterator it_ref_fingerprint;

	int m_extend_time_threshold = 1100;

	// we find the first timestamp and records the last timestamp
	// 记录所有WiFi序列的时间戳的 最大值 最小值
	long int min_timestamp = INFINITY;
	long int max_timestamp = -INFINITY;
	for (it_current_fingerprint = m_radio_fingerprints.begin(); it_current_fingerprint != m_radio_fingerprints.end(); it_current_fingerprint++)
	{
		if (it_current_fingerprint->first > max_timestamp)
		{
			max_timestamp = it_current_fingerprint->first;
		}

		if (it_current_fingerprint->first < min_timestamp)
		{
			min_timestamp = it_current_fingerprint->first;
		}
	}

	//以timeWindow为步进值 构建WiFi序列
	int timeWindow = 5000;
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

	std::cout << "WiFi指纹共有:" <<m_radio_fingerprints.size() << "个采样点 WiFi指纹序列共有:" << m_radio_fingerprints_extended.size() << "个" << std::endl; //记录WiFi指纹文件里总共在多少个时间点进行过采样 以及生成的WiFi序列 在多少个时间点有
	std::cout << "里程计共有：" <<m_robot_odometry.size() << "个采样点 groundtruth共有 "<< m_robot_ground_truth.size() << "个采样点" << std::endl;
	std::cout << "时间长度为:" << (double)(max_timestamp - min_timestamp) / 1000.0 <<" 秒"<< std::endl; //采样这个WiFi指纹文件花了多久

	std::cout << "分配 真实值.." << std::endl; //为每个WiFi指纹文件分配 groundtruth

	// 因为 里程计和WiFi指纹 里程计和WiFi序列 groundtruth和WiFi序列 的时间可能不一样 现在要把二者整合 要针对每个时间进行匹配
	// 整合： 里程计的x y z accumulated_dist存到WiFi指纹的类中 并记录 配对的里程计信息和WiFi指纹信息best_index_current
	for (it_current_fingerprint = m_radio_fingerprints.begin(); it_current_fingerprint != m_radio_fingerprints.end(); it_current_fingerprint++)
	{
		// 在 m_robot_odometry 寻找 时间戳 与m_radio_fingerprints的 当前时刻 最接近的值
		long int timestamp_current = it_current_fingerprint->first;

		double min_time_diff_current = INFINITY;
		int best_index_current = -1;
		for (it_current_odom = m_robot_odometry.begin(); it_current_odom != m_robot_odometry.end(); it_current_odom++)
		{
			double time_diff = fabs(((double)(timestamp_current)) / 1000.0 - it_current_odom->second.timestamp);
			if (time_diff <= min_time_diff_current)
			{
				best_index_current = it_current_odom->first;
				min_time_diff_current = time_diff;
			}
		}
		// 如果 存在最接近的 时间戳 且二者相差小于1秒 就把里程计读取的信息整合到m_radio_fingerprints中
		if (best_index_current >= 0 && min_time_diff_current <= 1.0)
		{
			m_radio_fingerprints[it_current_fingerprint->first].index_robot = best_index_current;
			m_radio_fingerprints[it_current_fingerprint->first].odom_x = m_robot_odometry[best_index_current].x;
			m_radio_fingerprints[it_current_fingerprint->first].odom_y = m_robot_odometry[best_index_current].y;
			m_radio_fingerprints[it_current_fingerprint->first].odom_z = m_robot_odometry[best_index_current].z;
			m_radio_fingerprints[it_current_fingerprint->first].accumulated_dist = m_robot_odometry[best_index_current].accumulated_dist;
		}
		else
		{
			m_radio_fingerprints[it_current_fingerprint->first].index_robot = -1;
			m_radio_fingerprints[it_current_fingerprint->first].accumulated_dist = -1;
		}
	}
	// 对里程计和WiFi序列进行同样的操作
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		long int timestamp_current = it_current_fingerprint->first;
		double min_time_diff_current = INFINITY;
		int best_index_current = -1;
		for (it_current_odom = m_robot_odometry.begin(); it_current_odom != m_robot_odometry.end(); it_current_odom++)
		{
			double time_diff = fabs(((double)(timestamp_current)) / 1000.0 - it_current_odom->second.timestamp);
			if (time_diff <= min_time_diff_current)
			{
				best_index_current = it_current_odom->first;
				min_time_diff_current = time_diff;
			}
		}
		if (best_index_current >= 0 && min_time_diff_current <= 1.0)
		{
			m_radio_fingerprints_extended[it_current_fingerprint->first].index_robot = best_index_current;
			m_radio_fingerprints_extended[it_current_fingerprint->first].odom_x = m_robot_odometry[best_index_current].x;
			m_radio_fingerprints_extended[it_current_fingerprint->first].odom_y = m_robot_odometry[best_index_current].y;
			m_radio_fingerprints_extended[it_current_fingerprint->first].odom_z = m_robot_odometry[best_index_current].z;
			m_radio_fingerprints_extended[it_current_fingerprint->first].accumulated_dist = m_robot_odometry[best_index_current].accumulated_dist;
		}
		else
		{
			m_radio_fingerprints_extended[it_current_fingerprint->first].index_robot = -1;
			m_radio_fingerprints_extended[it_current_fingerprint->first].accumulated_dist = -1;
		}
	}
	// 对groundtruth和WiFi序列进行同样的操作
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		long int timestamp_current = it_current_fingerprint->first;
		double min_time_diff_current = INFINITY;
		int best_index_current = -1;
		for (it_current_odom = m_robot_ground_truth.begin(); it_current_odom != m_robot_ground_truth.end(); it_current_odom++)
		{
			double time_diff = fabs(((double)(timestamp_current)) / 1000.0 - it_current_odom->second.timestamp);
			if (time_diff <= min_time_diff_current)
			{
				best_index_current = it_current_odom->first;
				min_time_diff_current = time_diff;
			}
		}
		if (best_index_current >= 0 && min_time_diff_current <= 1.0)
		{
			m_radio_fingerprints_extended[it_current_fingerprint->first].gt_found = 1;
			m_radio_fingerprints_extended[it_current_fingerprint->first].x = m_robot_ground_truth[best_index_current].x;
			m_radio_fingerprints_extended[it_current_fingerprint->first].y = m_robot_ground_truth[best_index_current].y;
			m_radio_fingerprints_extended[it_current_fingerprint->first].z = m_robot_ground_truth[best_index_current].z;
		}
		else
		{
			m_radio_fingerprints_extended[it_current_fingerprint->first].gt_found = -1;
		}
	}
	// 到此整合完毕

	double total_mac_address = 0;	// 求所采的WiFi指纹包中 总共有多少个mac地址	###为啥不用m_mac_counter.size() #因为m_mac_counter不会重新计算重复的mac地址 total_mac_address会重复计算
	double mean_mac_address = 0;	// 求平均每次采的 WiFi指纹 有多少个mac地址
	double total_count = 0;			// 求总共采了多少次WiFi指纹
	int min_mac_address = INFINITY; // 单个WiFi指纹下 最少有几个mac地址
	int max_mac_address = -1;		// 单个WiFi指纹下 最多有几个mac地址
	// 求平均每次采的 WiFi指纹 有多少个mac地址
	for (it_current_fingerprint = m_radio_fingerprints.begin(); it_current_fingerprint != m_radio_fingerprints.end(); it_current_fingerprint++)
	{
		total_mac_address = total_mac_address + it_current_fingerprint->second.wifi_scan.size();
		int scan_size = it_current_fingerprint->second.wifi_scan.size();
		if (scan_size > max_mac_address)
			max_mac_address = scan_size;

		if (scan_size < min_mac_address)
			min_mac_address = scan_size;

		total_count = total_count + 1;
	}

	mean_mac_address = total_mac_address / total_count;

	// 这里是对 lte 进行相同的操作
	double total_lte_mac_address = 0;
	double mean_lte_mac_address = 0;
	double total_lte_count = 0;
	int min_lte_mac_address = INFINITY;
	int max_lte_mac_address = -1;

	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		total_lte_mac_address = total_lte_mac_address + it_current_fingerprint->second.cell_scan.size();
		int scan_size = it_current_fingerprint->second.cell_scan.size();

		if (scan_size > max_lte_mac_address)
			max_lte_mac_address = scan_size;

		if (scan_size < min_lte_mac_address)
			min_lte_mac_address = scan_size;

		total_lte_count = total_lte_count + 1;
	}

	mean_lte_mac_address = total_lte_mac_address / total_lte_count;

	//对WiFi序列进行相同的操作
	double total_mac_address_extended = 0;
	double mean_mac_address_extended = 0;
	double total_count_extended = 0;
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		total_mac_address_extended = total_mac_address_extended + it_current_fingerprint->second.wifi_scan.size();
		total_count_extended = total_count_extended + 1;
	}

	mean_mac_address_extended = total_mac_address_extended / total_count_extended;

	// std::cout << "mean lte address:" << mean_lte_mac_address << std::endl;
	std::cout << "平均每次采的 WiFi指纹 有" << mean_lte_mac_address << "个 cell id " << std::endl;
	// std::cout << "lte mac min:" << min_lte_mac_address << ",lte mac max:" << max_lte_mac_address << std::endl;
	std::cout << "单次采的 WiFi指纹中 cell id最少有 " << min_lte_mac_address << " 个 单次采的 WiFi指纹中 cell id最多有" << max_lte_mac_address << std::endl;

	std::cout << "每次采样WiFi指纹平均会采到:" << mean_mac_address << "个mac地址, WiFi序列中每次采样平均有:" << mean_mac_address_extended << "个mac地址" << std::endl;
	// std::cout << "mean mac:" << mean_mac_address << ",mean_mac_address_extended:" << mean_mac_address_extended << std::endl;
	std::cout << "单次最少采到mac数最小值为:" << min_mac_address << ",单次最少采到mac数最大值为:" << max_mac_address << std::endl;
	// std::cout << "mac min:" << min_mac_address << ",mac max:" << max_mac_address << std::endl;
	// return 1;

	std::cout << "Modeling.." << std::endl;

	std::cout << "不重复的mac地址共有 " << m_mac_counter.size() << "个 cell id 共有： " << m_lte_counter.size() << "个" << std::endl;
	std::cout << "机器人共有 " << m_record_count << "个位姿信息" << std::endl;

	// we need to check the missing detection model
	double missing_dist_threshold = 10; //距离阈值 如果两个WiFi序列的accumulate_distance 之差 大于这个值 就说明不是同一个WiFi序列

	// we treat miss and extra the same
	std::vector<int> detection;		 //	用来计算 miss_detection_likelihood  存的是RSS值
	std::vector<int> miss_detection; //	用来计算 miss_detection_likelihood	存的是RSS值

	std::vector<FingerprintPair> fused_fingerprints_pair; // 记录经过checkMissingDetections处理过的指纹序列对

	std::map<int, double> miss_detection_likelihood; //###这是干嘛的	用来计算相似度的

	// std::map<long int, long int>::iterator it_mac_address;
	int min_rss = INFINITY;	 //最大RSS值
	int max_rss = -INFINITY; //最小RSS值
	int valid_min_rss = INFINITY;  //在detections和miss_detections中 有效的（可用的）RSS最小值
	int valid_max_rss = -INFINITY; //在detections和miss_detections中 有效的（可用的）RSS最大值

	//
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		for (it_ref_fingerprint = m_radio_fingerprints_extended.begin(); it_ref_fingerprint != m_radio_fingerprints_extended.end(); ++it_ref_fingerprint)
		// for(it_current_fingerprint = m_radio_fingerprints.begin(); it_current_fingerprint != m_radio_fingerprints.end(); it_current_fingerprint++)
		// for(it_ref_fingerprint = m_radio_fingerprints.begin();it_ref_fingerprint != m_radio_fingerprints.end(); ++it_ref_fingerprint)
		{
			std::map<string, int>::iterator it_mac_rss;

			for (it_mac_rss = it_current_fingerprint->second.wifi_scan.begin(); it_mac_rss != it_current_fingerprint->second.wifi_scan.end(); it_mac_rss++)
			{
				if (it_mac_rss->second > max_rss)
				{
					max_rss = it_mac_rss->second;
				}

				if (it_mac_rss->second < min_rss)
				{
					min_rss = it_mac_rss->second;
				}
			} //找出m_radio_fingerprints_extended中的最大RSS值和最小RSS值 感觉这一块可以写在这个for循环的外面

			if (it_current_fingerprint->first != it_ref_fingerprint->first)
			//遍历两个 时间戳 不同的 m_radio_fingerprints_extended
			{
				double acc_dist_current = it_current_fingerprint->second.accumulated_dist;		//accumulated_dist是一个单增的数
				double acc_dist_ref = it_ref_fingerprint->second.accumulated_dist;				//accumulated_dist是一个单增的数

				long int timestamp_current = it_current_fingerprint->first;
				long int timestamp_ref = it_ref_fingerprint->first;
				// we also check the timestamp between the two measurements, as most
				double time_diff = fabs(timestamp_ref - timestamp_current) / 1000.0; //计算时间戳的差

				if (acc_dist_current > 0 && acc_dist_ref > 0 && fabs(acc_dist_current - acc_dist_ref) < missing_dist_threshold)
				// 如果两个WiFi序列都非空 且 其中当前时刻的坐标之差 小于missing_dist_threshold阈值
				{
					if (time_diff > 7.0)
					// 如果时间差大于7秒 ###为什么是7秒
					{
						int pair_found = -1;
						for (int k = 0; k < fused_fingerprints_pair.size(); k++)
						{
							if ((fused_fingerprints_pair[k].f1 == it_current_fingerprint->first && fused_fingerprints_pair[k].f2 == it_ref_fingerprint->first) || (fused_fingerprints_pair[k].f2 == it_current_fingerprint->first && fused_fingerprints_pair[k].f1 == it_ref_fingerprint->first))
							// 如果在已经储存的 指纹序列对 中找到了当前 一对 则pair_found = 1
							{
								pair_found = 1;
								break;
							}
						}
						if (pair_found < 0)
						// 如果在已储存的 指纹序列对中 没有找到当前的一对 则
						{
							//	std::cout<<"ok:"<<it_current_fingerprint->first<<" "<<it_ref_fingerprint->first<<" "<<detection.size()<<" "<<miss_detection.size()<<std::endl;
							checkMissingDetections(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan, detection, miss_detection);
							// 把两个WiFi序列中 共同mac地址 的RSS 求平均 放到detection 把不共同的mac地址 的RSS 放到miss_detection
							FingerprintPair pair;						// 作用就是防止重复记录
							pair.f1 = it_current_fingerprint->first; 	// 把时间戳存进去
							pair.f2 = it_ref_fingerprint->first;	 	// 把时间戳存进去
							fused_fingerprints_pair.push_back(pair); 	// 每处理一对指纹对 就存进fused_fingerprints_pair中
						}
					}
				}
			}
		}
	}
	// 针对每个RSS值 在detection 和miss_detection 中 计算当前RSS值是否满足以binning_size+-2为范围的个数 分别得到num_detections和num_miss_detections
	// 然后在当前RSS值的条件下计算miss_detection_likelihood[RSS] = ratio = ((double)(num_miss_detections)) / ((double)(num_miss_detections + num_detections))
	// miss_detection_likelihood是在相同的RSS下 配对成功与配对未成功的比率
	// 用于后续的 相似度检测
	double binning_size = 2;
	for (int i = min_rss; i <= max_rss; i++)
	{
		int num_detections = 0;
		int num_miss_detections = 0;

		for (int k = 0; k < detection.size(); k++)
		{
			if (fabs(detection[k] - i) <= binning_size)
			{
				num_detections = num_detections + 1;
			}
		}

		for (int k = 0; k < miss_detection.size(); k++)
		{
			if (fabs(miss_detection[k] - i) <= binning_size)
			{
				num_miss_detections = num_miss_detections + 1;
			}
		}

		double ratio = ((double)(num_miss_detections)) / ((double)(num_miss_detections + num_detections));
		if (num_miss_detections + num_detections > 0)
		{
			// std::cout<<i<<" "<<num_miss_detections<<" "<<num_detections<<" "<<( (double)(num_miss_detections) )/( (double)(num_miss_detections+num_detections) )<<std::endl;
			miss_detection_likelihood[i] = ratio;
			if (i < valid_min_rss)
			{
				valid_min_rss = i;
			}

			if (i > valid_min_rss)
			{
				valid_max_rss = i;
			}
		}
	}

	valid_max_rss = -43; //为啥要这样 特定设置一个值
	////end of missing detection model
	std::cout<<min_rss<<" "<<max_rss<<" "<<valid_min_rss<<" "<<valid_max_rss<<std::endl;

	//开始建模  相似度 和 距离的模型

	//计算任意两个WiFi序列之间的相似度 并保存在m_similarty_distance_wifi里
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
		for (it_ref_fingerprint = m_radio_fingerprints_extended.begin(); it_ref_fingerprint != m_radio_fingerprints_extended.end(); ++it_ref_fingerprint)
		{
			if (it_current_fingerprint->first != it_ref_fingerprint->first)
			{
				double acc_dist_current = it_current_fingerprint->second.accumulated_dist;
				double acc_dist_ref = it_ref_fingerprint->second.accumulated_dist;

				double dist_threshold = 200; // 100

				if (acc_dist_current > 0 && acc_dist_ref > 0 && fabs(acc_dist_current - acc_dist_ref) < dist_threshold)
				{
					double previous_x = it_current_fingerprint->second.odom_x;
					double previous_y = it_current_fingerprint->second.odom_y;
					double previous_z = it_current_fingerprint->second.odom_z;

					double x = it_ref_fingerprint->second.odom_x;
					double y = it_ref_fingerprint->second.odom_y;
					double z = it_ref_fingerprint->second.odom_z;

					double delta_x = x - previous_x;
					double delta_y = y - previous_y;
					double delta_z = z - previous_z;
					double position_diff = sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

					double sim_wifi_cosine = 0;
					double sim_wifi_gaussian = 0;
					double sim_wifi_common = 0;
					double sim_wifi_gaussian_penalty = 0;

					double sim_wifi;

					sim_wifi_cosine = getCosineSimilarity(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan);

					sim_wifi_common = getCommonDetections(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan);

					// sim_wifi_gaussian=getGaussianSimilarityOriginal(it_current_fingerprint->second.wifi_scan,it_ref_fingerprint->second.wifi_scan);

					sim_wifi_gaussian_penalty = getGaussianSimilarityWithPenalty(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan, valid_min_rss, valid_max_rss, miss_detection_likelihood);

					sim_wifi = sim_wifi_cosine * sim_wifi_gaussian_penalty * sim_wifi_common;
					// sim_wifi=sim_wifi_gaussian;

					double sim_lte = getCosineSimilarity(it_current_fingerprint->second.cell_scan, it_ref_fingerprint->second.cell_scan);

					SimilartyDistance sim_dis_wifi;
					sim_dis_wifi.similarity = sim_wifi;
					sim_dis_wifi.distance = position_diff;
					sim_dis_wifi.delta_x = delta_x;
					sim_dis_wifi.delta_y = delta_y;
					sim_dis_wifi.delta_z = delta_z;

					SimilartyDistance sim_dis_lte;
					sim_dis_lte.similarity = sim_lte;
					sim_dis_lte.distance = position_diff;
					sim_dis_lte.delta_x = delta_x;
					sim_dis_lte.delta_y = delta_y;
					sim_dis_lte.delta_z = delta_z;

					if (sim_wifi > 0)
					{
						// std::cout<<sim<<" "<<position_diff<<" "<<x<<" "<<y<<" "<<z<<" "<<previous_x<<" "<<previous_y<<" "<<previous_z<<std::endl;
						//	std::cout<<"wifi:"<<sim_wifi<<" "<<position_diff<<std::endl;
						m_similarty_distance_wifi.push_back(sim_dis_wifi);

						if (sim_wifi > 1.0)
						{
							std::cout << "sample:" << sim_wifi << " " << position_diff << std::endl;
						}
					}

					if (sim_lte > 0)
					{
						// std::cout<<sim<<" "<<position_diff<<" "<<x<<" "<<y<<" "<<z<<" "<<previous_x<<" "<<previous_y<<" "<<previous_z<<std::endl;
						m_similarty_distance_lte.push_back(sim_dis_lte);
					}
				}
			}
		}

	double binning_resolution = 0.05;
	double half_binning_resolution = binning_resolution * 0.5; // 0.025
	double look_up_table = 0.01;

	std::cout << "binning..." << std::endl;
	// for wifi binning 不确定性模型的训练  可以理解成 把相似度转换成距离
	// 根据相似度 从1 到0 每次计算相似度在min_sim max_sim之间的平均距离和方差 并把 相似度 在min_sim max_sim之间的方差 存入m_similarty_variance_wifi中
	// min_sim max_sim是一个动态的范围
	for (double b = 1; b >= 0.0; b = b - look_up_table)
	{
		std::vector<double> v_distance;

		for (int i = 0; i < m_similarty_distance_wifi.size(); i++)		//遍历任意两个WiFi序列相似度的数组
		{
			double min_sim = b - half_binning_resolution; // b - 0.025
			double max_sim = b + half_binning_resolution; // b + 0.025

			if (b >= 1 - half_binning_resolution) // 0.975
			{
				min_sim = 1 - binning_resolution; // 0.95
			}
			if (b <= half_binning_resolution) // 0.025
			{
				max_sim = binning_resolution; // 0.05
			}
			if (m_similarty_distance_wifi[i].similarity >= min_sim && m_similarty_distance_wifi[i].similarity <= max_sim)
			{
				v_distance.push_back(m_similarty_distance_wifi[i].distance);		// 先记录满足 相似度在min_sim max_sim之间的距离
			}
		}
		double mean_distance = 0; 													// 再计算 相似度在min_sim max_sim之间的平均距离
		double variance = 0;														// 再计算 相似度在min_sim max_sim之间的方差

		for (int i = 0; i < v_distance.size(); i++)
		{
			mean_distance = mean_distance + v_distance[i];
		}

		if (v_distance.size() > 0)
		{
			mean_distance = mean_distance / ((double)(v_distance.size()));

			for (int i = 0; i < v_distance.size(); i++)
			{
				variance = variance + (v_distance[i] - mean_distance) * (v_distance[i] - mean_distance);
			}
			variance = variance / ((double)(v_distance.size()));

			SimilartyVariance similarty_variance;
			similarty_variance.similarity = b;
			similarty_variance.variance = variance;
			similarty_variance.mean = mean_distance;

			m_similarty_variance_wifi.push_back(similarty_variance);				// 再把 相似度在min_sim max_sim之间的 相似度 距离的方差 平均距离 存入m_similarty_variance_wifi中

			// std::cout<<"wifi:"<<b<<" "<<similarty_variance.variance<<" "<<sqrt(similarty_variance.variance)<<" "<<v_distance.size()<<" "<<mean_distance<<std::endl;
			//	std::cout<<"wifi:"<<b<<" "<<mean_distance<<" "<<sqrt(similarty_variance.variance)<<" "<<similarty_variance.variance<<" "<<v_distance.size()<<std::endl;
		}
	}

	// for lte binning 同上
	for (double b = 1; b >= 0.0; b = b - look_up_table)
	{

		std::vector<double> v_distance;

		for (int i = 0; i < m_similarty_distance_lte.size(); i++)
		{
			double min_sim = b - half_binning_resolution;
			double max_sim = b + half_binning_resolution;

			if (b >= 1 - half_binning_resolution)
			{
				min_sim = 1 - binning_resolution;
			}

			if (b <= half_binning_resolution)
			{
				max_sim = binning_resolution;
			}

			if (m_similarty_distance_lte[i].similarity >= min_sim && m_similarty_distance_lte[i].similarity <= max_sim)
			{
				v_distance.push_back(m_similarty_distance_lte[i].distance);
			}
		}
		double mean_distance = 0;
		double variance = 0;

		for (int i = 0; i < v_distance.size(); i++)
		{
			mean_distance = mean_distance + v_distance[i];
		}

		if (v_distance.size() > 0)
		{

			mean_distance = mean_distance / ((double)(v_distance.size()));

			for (int i = 0; i < v_distance.size(); i++)
			{
				variance = variance + (v_distance[i] - mean_distance) * (v_distance[i] - mean_distance);
			}
			variance = variance / ((double)(v_distance.size()));

			SimilartyVariance similarty_variance;
			similarty_variance.similarity = b;
			similarty_variance.variance = variance;
			similarty_variance.mean = mean_distance;

			m_similarty_variance_lte.push_back(similarty_variance);

			//	std::cout<<"lte:"<<b<<" "<<mean_distance<<" "<<sqrt(similarty_variance.variance)<<" "<<similarty_variance.variance<<" "<<v_distance.size()<<std::endl;
		}
	}

	//结束 相似度 和 距离的 建模

	// return 1;

	std::map<string, int>::iterator it_mac_address;

	std::map<string, int> mac_count; // mac address to the number of detections 	mac地址转换成这个mac地址被检测到次数		first mac地址 second 这个mac地址被检测到次数

	std::map<string, int> mac_node_id; // mac address -> node id in the graph 		mac地址转换成图的节点id 从0开始

	std::map<int, string> node_id_mac; // node id in the graph->mac address			图节点id转换成mac地址

	for (it_current_fingerprint = m_radio_fingerprints.begin(); it_current_fingerprint != m_radio_fingerprints.end(); it_current_fingerprint++)
	// 遍历所有时间戳 的所有wifi_scan 的所有mac地址
	{
		std::map<string, int> rss_measure = it_current_fingerprint->second.wifi_scan; // rss measure
		for (it_mac_address = rss_measure.begin(); it_mac_address != rss_measure.end(); it_mac_address++)
		{
			mac_count[it_mac_address->first] = mac_count[it_mac_address->first] + 1;	// 把mac_count中 这个mac地址下的 被检测到的次数+1
		}
	}

	m_odom_record = m_record_count;
	for (it_mac_address = mac_count.begin(); it_mac_address != mac_count.end(); it_mac_address++)
	// mac地址转换成图的节点id
	{
		mac_node_id[it_mac_address->first] = m_record_count;
		m_record_count = m_record_count + 1;
	}

	for (it_mac_address = mac_node_id.begin(); it_mac_address != mac_node_id.end(); it_mac_address++)
	//图节点id转换成mac地址
	{
		node_id_mac[it_mac_address->second] = it_mac_address->first;
	}

	plot_similarity_model(m_similarty_variance_wifi);			// 这里是不是可以放到前面

	// Set up the optimiser
	SparseOptimizer optimizer;
	optimizer.setVerbose(false);

	// Create the block solver - the dimensions are specified because 3D observations marginalise to a 3D estimate
	// 创建块解算器-指定尺寸是因为三维观察值边缘化为三维估计值
	typedef BlockSolver<BlockSolverX> SlamBlockSolver;	//动态的BlockSolverX = BlockSolverPL<Eigen::Dynamic, Eigen::Dynamic>;
	OptimizationAlgorithmLevenberg *solver = new OptimizationAlgorithmLevenberg(
		g2o::make_unique<SlamBlockSolver>(
			g2o::make_unique<LinearSolverCSparse<SlamBlockSolver::PoseMatrixType>>()));

	optimizer.setAlgorithm(solver);

	// add the parameter representing the sensor offset
	// 添加表示传感器偏移的参数
	ParameterSE3Offset *sensorOffset = new ParameterSE3Offset;
	sensorOffset->setId(0);
	optimizer.addParameter(sensorOffset);

	/*
	//the graph-based slam solver
	SparseOptimizer optimizer;
	//Levenberg,block solver
	typedef BlockSolver< BlockSolverX >  SlamBlockSolver;
	typedef LinearSolverCSparse<SlamBlockSolver::PoseMatrixType> SlamLinearSolver;
	// allocating the optimizer
	SlamLinearSolver* linearSolver = new SlamLinearSolver();
	linearSolver->setBlockOrdering(false);
	SlamBlockSolver* blockSolver = new SlamBlockSolver(linearSolver);
	OptimizationAlgorithmLevenberg* solver = new OptimizationAlgorithmLevenberg(blockSolver);
	optimizer.setAlgorithm(solver);

	// add the parameter representing the sensor offset
	ParameterSE3Offset* sensorOffset = new ParameterSE3Offset;
	Isometry3D sensorOffsetTransf=Isometry3D::Identity();
	sensorOffset->setOffset(sensorOffsetTransf);
	sensorOffset->setId(0);
	optimizer.addParameter(sensorOffset);
	 */

	// 时间约束
	long int time_constrain_long = min_timestamp + 30.0 * (max_timestamp - min_timestamp) / 3.0;
	double time_constrain = ((double)(time_constrain_long)) / 1000.0;

	// add the nodes in the graph
	// 位姿节点
	for (it_current_odom = m_robot_odometry.begin(); it_current_odom != m_robot_odometry.end(); it_current_odom++)
	{
		if (it_current_odom->second.timestamp > time_constrain)
			continue;		//跳出循环
		double x = it_current_odom->second.x;
		double y = it_current_odom->second.y;
		double z = it_current_odom->second.z;

		double qx = it_current_odom->second.qx;
		double qy = it_current_odom->second.qy;
		double qz = it_current_odom->second.qz;
		double qw = it_current_odom->second.qw;

		Eigen::Vector3d trans(x, y, z);
		Eigen::Quaterniond q(qw, qx, qy, qz); // w,x,y,z
		g2o::SE3Quat current(q, trans);

		VertexSE3 *robot = new VertexSE3;
		robot->setId(it_current_odom->first);	// 设置当前时刻的位姿

		robot->setEstimate(current);
		optimizer.addVertex(robot);
	}

	// we add the odometry-based constraints
	// 加边 上一秒与这一秒的 里程计信息作为边
	for (it_current_odom = m_robot_odometry.begin(); it_current_odom != m_robot_odometry.end(); it_current_odom++)
	{
		if (it_current_odom->second.timestamp > time_constrain)
			continue;
		int previous_id = it_current_odom->first - 1;	//###上一秒？

		std::map<int, RobotOdom>::iterator it_previous_odom;

		it_previous_odom = m_robot_odometry.find(previous_id);

		if (it_previous_odom != m_robot_odometry.end())
		{
			EdgeSE3 *odometry = new EdgeSE3;
			odometry->vertices()[0] = optimizer.vertex(it_previous_odom->first);	//odometry->vertices():returns the pointer to the ith vertex connected to the hyper-edge.
			odometry->vertices()[1] = optimizer.vertex(it_current_odom->first);

			double previous_x = it_previous_odom->second.x;
			double previous_y = it_previous_odom->second.y;
			double previous_z = it_previous_odom->second.z;
			double previous_qx = it_previous_odom->second.qx;
			double previous_qy = it_previous_odom->second.qy;
			double previous_qz = it_previous_odom->second.qz;
			double previous_qw = it_previous_odom->second.qw;

			double x = it_current_odom->second.x;
			double y = it_current_odom->second.y;
			double z = it_current_odom->second.z;
			double qx = it_current_odom->second.qx;
			double qy = it_current_odom->second.qy;
			double qz = it_current_odom->second.qz;
			double qw = it_current_odom->second.qw;

			Eigen::Vector3d previous_trans(previous_x, previous_y, previous_z);
			Eigen::Quaterniond previous_q(previous_qw, previous_qx, previous_qy, previous_qz); // w,x,y,z

			Eigen::Vector3d trans(x, y, z);
			Eigen::Quaterniond q(qw, qx, qy, qz); // w,x,y,z

			g2o::SE3Quat previous(previous_q, previous_trans);
			g2o::SE3Quat current(q, trans);

			g2o::SE3Quat relative_odom = previous.inverse() * current; // relative transformtion T=inverse(A)*B,
			Eigen::Quaterniond qd_relative_odom = relative_odom.rotation();
			Eigen::Quaternion<float> q_relative_odom = relative_odom.rotation().cast<float>();
			Eigen::Vector3d t_relative_odom = relative_odom.translation();

			auto euler = qd_relative_odom.toRotationMatrix().eulerAngles(0, 1, 2);

			// std::cout<<x<<" "<<y<<" "<<z<<std::endl;
			// std::cout<<"tx ty tz:"<<t_relative_odom.x()<<" "<<t_relative_odom.y()<<" "<<t_relative_odom.z()<<std::endl;
			// std::cout<<"euler angles:"<<temp_euler[0]<<" "<<temp_euler[1]<<" "<<temp_euler[2]<<std::endl;

			/*
			//default noise of odometry
			double tran_noise=0.1;//0.05
			double rotation_noise=0.2*M_PI/180.0;//0.2

			Eigen::Matrix<double,6,6> covariance;
			covariance.fill(0);

			covariance(0,0)=carmen_square(fabs(t_relative_odom(0)*tran_noise)+0.01);//0.0001
			covariance(1,1)=carmen_square(fabs(t_relative_odom(1)*tran_noise)+0.01);
			covariance(2,2)=carmen_square(fabs(t_relative_odom(2)*tran_noise)+0.01);

			covariance(3,3)=carmen_square(fabs(euler[0]*rotation_noise)+0.01*M_PI/180.0);
			covariance(4,4)=carmen_square(fabs(euler[1]*rotation_noise)+0.01*M_PI/180.0);
			covariance(5,5)=carmen_square(fabs(euler[2]*rotation_noise)+0.01*M_PI/180.0);

Eigen::Matrix<double,6,6> information=Eigen::Matrix<double,6,6>::Identity();


			information(0,0)=1.0/covariance(0,0);

			information(1,1)=1.0/covariance(1,1);


			information(2,2)=0;
			information(3,3)=0;
			information(4,4)=0;
			information(5,5)=1.0/covariance(5,5);
			 */

			double delta_x = t_relative_odom(1);
			double delta_y = t_relative_odom(2);
			double delta_theta = fabs(carmen_normalize_theta(euler[2]));

			// we need to add odom based constrain
			// default noise of odometrypure_odom_constrain
			// 噪声
			double tran_noise = 0.1;					// 0.05
			double rotation_noise = 0.2 * M_PI / 180.0; // 0.2

			Eigen::Matrix<double, 6, 6> covariance;
			covariance.fill(0);

			covariance(0, 0) = carmen_square(fabs(delta_x * tran_noise) + fabs(delta_y * 0.01) + 0.001);
			covariance(1, 1) = carmen_square(fabs(delta_y * tran_noise) + fabs(0.01 * delta_x) + 0.001);
			covariance(5, 5) = carmen_square(fabs(delta_theta * rotation_noise) + fabs(delta_x * rotation_noise * 0.01) + 0.01 * M_PI / 180.0);

			Eigen::Matrix<double, 6, 6> information = Eigen::Matrix<double, 6, 6>::Identity();
			// 信息矩阵
			information(0, 0) = 1.0 / covariance(0, 0);

			information(1, 1) = 1.0 / covariance(1, 1);

			information(2, 2) = 0;
			information(3, 3) = 0;
			information(4, 4) = 0;
			information(5, 5) = 1.0 / covariance(5, 5);

			odometry->setMeasurement(relative_odom);
			odometry->setInformation(information);
			optimizer.addEdge(odometry);
		}
	}

	int num_constrain = 0;
	int combined_constrains = 0;

	// 进行WiFi指纹序列 闭环检测 作为图优化中的边
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
		for (it_ref_fingerprint = m_radio_fingerprints_extended.begin(); it_ref_fingerprint != m_radio_fingerprints_extended.end(); ++it_ref_fingerprint)
		{
			// we check the similarity between this two fingerprints

			double current_time = ((double)(it_current_fingerprint->first)) / 1000.0;
			double ref_time = ((double)(it_ref_fingerprint->first)) / 1000.0;

			if (current_time > time_constrain || ref_time > time_constrain)
				continue;

			if (it_current_fingerprint->first == it_ref_fingerprint->first)
				continue;

			double acc_current_dist = it_current_fingerprint->second.accumulated_dist;

			double acc_ref_dist = it_ref_fingerprint->second.accumulated_dist;

			double distance_diff = fabs(acc_ref_dist - acc_current_dist);

			int robot_id_current = it_current_fingerprint->second.index_robot;	//robot_id_current是 这个WiFi序列对应的里程计中的时间戳
			int robot_id_ref = it_ref_fingerprint->second.index_robot;

			double likelihood = carmen_uniform_random(0, 1);

			double previous_x = it_ref_fingerprint->second.odom_x;
			double previous_y = it_ref_fingerprint->second.odom_y;
			double previous_z = it_ref_fingerprint->second.odom_z;

			double x = it_current_fingerprint->second.odom_x;
			double y = it_current_fingerprint->second.odom_y;
			double z = it_current_fingerprint->second.odom_z;
			double delta_x = x - previous_x;
			double delta_y = y - previous_y;
			double delta_z = z - previous_z;
			double position_diff = sqrt(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);

			if (robot_id_current >= 0 && robot_id_ref >= 0 && robot_id_current > robot_id_ref && distance_diff >= 100 && likelihood < 1.2)
			// 两个WiFi序列都有效 且 WiFi序列1的时间戳必须比大于WiFi序列2的时间戳 且累计距离差需要很大
			{
				double sim_lte = getCosineSimilarity(it_current_fingerprint->second.cell_scan, it_ref_fingerprint->second.cell_scan);

				double sim_wifi = 0;
				double sim_wifi_cosine = 0;
				double sim_wifi_gaussian = 0;
				double sim_wifi_common = 0;
				double sim_wifi_gaussian_penalty = 0;

				sim_wifi_cosine = getCosineSimilarity(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan);

				// sim_wifi_gaussian=getGaussianSimilarityOriginal(it_current_fingerprint->second.wifi_scan,it_ref_fingerprint->second.wifi_scan);

				sim_wifi_common = getCommonDetections(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan);

				sim_wifi_gaussian_penalty = getGaussianSimilarityWithPenalty(it_current_fingerprint->second.wifi_scan, it_ref_fingerprint->second.wifi_scan, valid_min_rss, valid_max_rss, miss_detection_likelihood);

				sim_wifi = sim_wifi_cosine * sim_wifi_gaussian_penalty * sim_wifi_common;
				// sim_wifi=sim_wifi_gaussian;

				double lte_sim_thresh = 1.8;
				double wifi_sim_thresh = m_sim_threshold;	// 从命令行获取的

				double combined_variance;
				double combined_mean;

				double lte_mean;
				double lte_variance;

				double wifi_mean;
				double wifi_variance;

				// get the lte mean and variance
				// 把对应这对WiFi序列的相似度的 方差的平均值 和方差？记录
				if (sim_lte >= lte_sim_thresh)
				{
					// find the uncertainty based on the previously learned model
					int similarity_table_index = 0;
					double min_sim_diff = INFINITY;
					for (int s = 0; s < m_similarty_variance_lte.size(); s++)
					{
						if (fabs(sim_lte - m_similarty_variance_lte[s].similarity) < min_sim_diff)
						// 找出 sim_lte 与 m_similarty_variance_lte中 最小的差值 并记录该位置
						{
							min_sim_diff = fabs(sim_lte - m_similarty_variance_lte[s].similarity);
							similarity_table_index = s;
						}
					}
					lte_mean = m_similarty_variance_lte[similarity_table_index].mean;			// 把这对应的相似度 转换成 距离的均值
					lte_variance = m_similarty_variance_lte[similarity_table_index].variance;	// 把对应的相似度 转换成 距离的方差
				}

				// get the wifi mean and variance
				// 把对应这对WiFi序列的相似度的 方差的平均值 和方差？记录
				if (sim_wifi >= wifi_sim_thresh)
				{
					// find the uncertainty based on the previously learned model
					int similarity_table_index = 0;
					double min_sim_diff = INFINITY;
					for (int s = 0; s < m_similarty_variance_wifi.size(); s++)
					{
						if (fabs(sim_wifi - m_similarty_variance_wifi[s].similarity) < min_sim_diff)
						{
							min_sim_diff = fabs(sim_wifi - m_similarty_variance_wifi[s].similarity);
							similarity_table_index = s;
						}
					}
					wifi_mean = m_similarty_variance_wifi[similarity_table_index].mean;			// 把对应的相似度转换成 距离的均值
					wifi_variance = m_similarty_variance_wifi[similarity_table_index].variance;	// 把对应的相似度转换成 距离的方差
				}

				int valid = -1;

				if (sim_lte >= lte_sim_thresh && sim_wifi >= wifi_sim_thresh)
				{
					// we need to combined the two measures
					double sum_variance = wifi_variance + lte_variance;							// 求方差的和

					// combined_mean=lte_mean*(wifi_variance/sum_variance)+wifi_mean*(lte_variance/sum_variance);
					// combined_variance=1.0/(1.0/wifi_variance+1.0/lte_variance);

					combined_mean = wifi_mean;
					combined_variance = wifi_variance;
					valid = 1;

					combined_constrains = combined_constrains + 1;	//两种条件都满足的约束个数

					// std::cout<<"Combined constrains:"<<combined_constrains<<",combined:"<<combined_mean<<" "<<combined_variance<<",lte:"<<lte_mean<<" "<<lte_variance<<",wifi:"<<wifi_mean<<" "<<wifi_variance<<std::endl;
					// std::cout<<num_constrain<<", combined from:"<<robot_id_current<<",to:"<<robot_id_ref<<" "<<position_diff<<",lte similarity:"<<sim_lte<<",wifi similarity:"<<sim_wifi<<std::endl;
				}
				else
				{
					if (sim_lte >= lte_sim_thresh)
					{
						combined_mean = lte_mean;
						combined_variance = lte_variance;
						valid = 1;
						// std::cout<<num_constrain<<", LTE from:"<<robot_id_current<<",to:"<<robot_id_ref<<" "<<position_diff<<",lte similarity:"<<sim_lte<<",wifi similarity:"<<sim_wifi<<std::endl;
					}
					else if (sim_wifi >= wifi_sim_thresh)
					{
						combined_mean = wifi_mean;
						combined_variance = wifi_variance;
						valid = 1;
						// std::cout<<num_constrain<<", Wifi from:"<<robot_id_current<<",to:"<<robot_id_ref<<" "<<position_diff<<",lte similarity:"<<sim_lte<<",wifi similarity:"<<sim_wifi<<std::endl;
					}
				}

				// lte 0.9
				// wifi 0.8
				if (valid > 0)
				{

					double inv_variance = 1.0 / combined_variance;

					EdgeSE3Distance *observation = new EdgeSE3Distance;
					observation->setMeasurement(combined_mean);

					observation->vertices()[0] = optimizer.vertex(robot_id_current);
					observation->vertices()[1] = optimizer.vertex(robot_id_ref);
					observation->setParameterId(0, 0);

					// set the information matrix
					Eigen::Matrix<double, 1, 1> information_matrix = inv_variance * Eigen::Matrix<double, 1, 1>::Identity();
					observation->setInformation(information_matrix);

					optimizer.addEdge(observation);
					num_constrain = num_constrain + 1;
					// std::cout<<num_constrain<<",from:"<<robot_id_current<<",to:"<<robot_id_ref<<" "<<position_diff<<std::endl;
				}
			}
		}

	// prepare and run the optimization
	// fix the first robot pose to account for gauge freedom  固定第一个机器人的姿势，以考虑到尺度自由度。
	std::cout << "Setting first robot pose.." << std::endl;
	VertexSE3 *firstRobotPose = dynamic_cast<VertexSE3 *>(optimizer.vertex(0));
	firstRobotPose->setFixed(true);			//设置是否固定，第一帧固定
	firstRobotPose->setMarginalized(true);	//是否边缘化（以便稀疏化求解）
											//G2O 中对路标点设置边缘化(pPoint->setMarginalized(true))是为了 在计算求解过程中，先消去路标点变量，
											//实现先求解相机位姿，然后再利用求解出来的相机 位姿反过来计算路标点的过程，目的是为了加速求解，并非真的将路标点给边缘化掉;

	optimizer.setVerbose(false);			//打开调试输出 否

	// save the raw pose before optimization
	OptimizableGraph::VertexIDMap graph_vertices = optimizer.vertices();	//std::unordered_map<int, Vertex*>     VertexIDMap;
	OptimizableGraph::VertexIDMap::iterator it_map;
	for (it_map = graph_vertices.begin(); it_map != graph_vertices.end(); ++it_map)
	{
		if (it_map->first < m_odom_record)
		{
			// it is a odom node
			VertexSE3 *vertex = dynamic_cast<VertexSE3 *>(it_map->second);	//unordered_map<int, Vertex*>
			g2o::SE3Quat pose_se3quat = vertex->estimateAsSE3Quat();
			m_raw_se3quat_pose[it_map->first] = pose_se3quat;
		}
		else
		{
			// it is a ap node
		}
	}

	std::map<std::string, int>::iterator it_mac;

	for (it_mac = m_mac_counter.begin(); it_mac != m_mac_counter.end(); ++it_mac)
	{
		// std::cout<<it_mac->first<<" "<<it_mac->second<<std::endl;
	}

	std::cout << "num_constrain:" << num_constrain << ",combined:" << combined_constrains << std::endl;

	std::cout << "Optimizing" << std::endl;
	optimizer.initializeOptimization();
	optimizer.optimize(40);					//指定迭代次数:40次
	std::cout << "done." << std::endl;

	std::vector<std::pair<int, int>> blockIndices;

	std::vector<int> nodeIDs;

	// save the optimized poses
	graph_vertices = optimizer.vertices();
	for (it_map = graph_vertices.begin(); it_map != graph_vertices.end(); ++it_map)
	{
		if (it_map->first < m_odom_record)
		// 一部分是机器人位姿
		{
			VertexSE3 *vertex = dynamic_cast<VertexSE3 *>(it_map->second);
			g2o::SE3Quat pose_se3quat = vertex->estimateAsSE3Quat();
			m_optimized_se3quat_pose[it_map->first] = pose_se3quat;

			RobotOdom optimized_odom;
			optimized_odom.x = pose_se3quat.translation()[0];
			optimized_odom.y = pose_se3quat.translation()[1];
			optimized_odom.z = pose_se3quat.translation()[2];
			optimized_odom.timestamp = m_robot_odometry[it_map->first].timestamp;

			m_robot_odometry_optimized[it_map->first] = optimized_odom;
		}
		else
		// 一部分是路标
		{
			VertexPointXYZ *vertex = dynamic_cast<VertexPointXYZ *>(it_map->second);
			carmen_6d_point_t pose;
			pose.x = vertex->estimate().x();
			pose.y = vertex->estimate().y();
			pose.z = vertex->estimate().z();
			m_v_landmark_pose.push_back(pose);

			std::string mac = node_id_mac[it_map->first];
			m_landmark_pose_map[mac] = pose;
			std::pair<int, int> indexParams;
			indexParams.first = vertex->hessianIndex();
			indexParams.second = vertex->hessianIndex();
			blockIndices.push_back(indexParams);
			nodeIDs.push_back(it_map->first);

			/*
			SparseBlockMatrix<Eigen::MatrixXd> spinv;

			if(optimizer.computeMarginals(spinv, vertex))
			{
				//   std::cout << spinv << std::endl;
				Eigen::MatrixXd m = *spinv.block(vertex->hessianIndex(), vertex->hessianIndex());

				std::cout << "Param covariance" << std::endl;
				std::cout << m << std::endl;
				//std::cout<<"m "<<"rows="<<m.rows()<<"cols="<<m.cols()<<"coefficeints="<<m.size()<<std::endl;

				for(int a=0;a<m.rows();a++)
					for(int b=0;b<m.cols();b++)
					{
						//std::cout<<a<<" "<<b<<" "<<m(a,b)<<std::endl;

					}
				//std::cout << "rotationDeterminant: " << m.block<3,3>(0,0).determinant() << std::endl;
				//std::cout << "translationDeterminant: " << m.block<3,3>(3,3).determinant()  << std::endl;
				std::string mac_address=node_id_mac[it_map->first];
				std::cout<<mac_address<<" "<<mac_count[mac_address]<<std::endl;*/
		}
	}

	// we find the mac address->and distance

	/*
	SparseBlockMatrix<Eigen::MatrixXd> spinv;

	if(optimizer.computeMarginals(spinv, blockIndices))
	{
		for(int i=0;i<blockIndices.size();i++)
		{
			//   std::cout << spinv << std::endl;
			Eigen::MatrixXd m = *spinv.block(blockIndices[i].first, blockIndices[i].second);

			std::cout << "Param covariance" << std::endl;
			std::cout << m << std::endl;

			std::string mac_address=node_id_mac[nodeIDs[i]];
			//std::cout<<mac_address<<" "<<mac_count[mac_address]<<" "<<m_v_landmark_pose[i].x<<" "<<m_v_landmark_pose[i].y<<" "<<m_v_landmark_pose[i].z<<std::endl;

		}



	}
	 */
	// we plot the raw pose, optimized pose and the constrains
	// plot_track(m_raw_se3quat_pose,m_optimized_se3quat_pose);
	// sleep(1000000000);
	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); it_current_fingerprint++)
	{
		int index_robot = it_current_fingerprint->second.index_robot;
		// we check the every location in the reference recording and record the estimated pose
		std::map<int, RobotOdom>::iterator it_odom;

		it_odom = m_robot_odometry_optimized.find(index_robot);

		if (it_odom != m_robot_odometry_optimized.end())
		{
			// we find this odom and record the optimized pose
			m_radio_fingerprints_optimized[it_current_fingerprint->first].x = it_odom->second.x;
			m_radio_fingerprints_optimized[it_current_fingerprint->first].y = it_odom->second.y;
			m_radio_fingerprints_optimized[it_current_fingerprint->first].z = it_odom->second.z;
		}
	}

	double first_gt_x = 0;
	double first_gt_y = 0;

	double first_optimized_x = 0;
	double first_optimized_y = 0;

	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); ++it_current_fingerprint)
	{
		// we find the ground truth
		it_ref_fingerprint = m_radio_fingerprints_optimized.find(it_current_fingerprint->first);

		if (it_ref_fingerprint != m_radio_fingerprints_optimized.end() && it_current_fingerprint->second.gt_found > 0)
		{
			first_gt_x = it_current_fingerprint->second.x;
			first_gt_y = it_current_fingerprint->second.y;

			first_optimized_x = it_ref_fingerprint->second.x;

			first_optimized_y = it_ref_fingerprint->second.y;
		}
	}

	// double x_offset=mean_gt_x-mean_optimized_x;
	// double y_offset=mean_gt_y-mean_gt_y;

	double x_offset = first_gt_x - first_optimized_x;
	double y_offset = first_gt_y - first_optimized_y;

	// plot_trackWithoutLandmark(m_raw_se3quat_pose,m_optimized_se3quat_pose);	//#####

	//	sleep(999999999999999999);

	std::cout << "first gt x:" << first_gt_x << ", first gt y:" << first_gt_y << std::endl;
	std::cout << "first optimized x:" << first_optimized_x << ", first optimized y:" << first_optimized_y << std::endl;

	double min_error = INFINITY;
	double variance;

	double best_d_rotation;
	double best_d_x_offset;
	double best_d_y_offset;

	// x offset:-0.1,y offset:0.7 rotation:-6.93889e-17

	for (double d_rotation = -3; d_rotation <= 3; d_rotation = d_rotation + 0.2)
		for (double d_x_offset = -100; d_x_offset <= 100; d_x_offset = d_x_offset + 5)
			for (double d_y_offset = -100; d_y_offset <= 100; d_y_offset = d_y_offset + 5)
			{
				std::vector<double> v_error; // analysis of error

				for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); ++it_current_fingerprint)
				{
					// we find the ground truth

					it_ref_fingerprint = m_radio_fingerprints_optimized.find(it_current_fingerprint->first);

					if (it_ref_fingerprint != m_radio_fingerprints_optimized.end() && it_current_fingerprint->second.gt_found > 0)
					{
						carmen_point_t gt;
						gt.x = it_current_fingerprint->second.x;
						gt.y = it_current_fingerprint->second.y;

						carmen_point_t position;
						position.x = it_ref_fingerprint->second.x;
						position.y = it_ref_fingerprint->second.y;

						double e_x = position.x * cos(d_rotation) - position.y * sin(d_rotation) + d_x_offset + x_offset;
						double e_y = position.x * sin(d_rotation) + position.y * cos(d_rotation) + d_y_offset + y_offset;

						double error = sqrt((gt.x - e_x) * (gt.x - e_x) + (gt.y - e_y) * (gt.y - e_y));
						v_error.push_back(error);
					}
				}

				// caculate the mean and standard deviation of mean
				double sum_error = 0;
				double sum_deviation = 0;
				double average_error = 0;
				double deviation_error = 0;

				for (int i = 0; i < v_error.size(); i++)
				{
					sum_error = sum_error + v_error[i];
				}

				average_error = sum_error / ((double)(v_error.size()));

				for (int i = 0; i < v_error.size(); i++)
				{
					sum_deviation = sum_deviation + (v_error[i] - average_error) * (v_error[i] - average_error);
				}

				deviation_error = sqrt(sum_deviation / ((double)(v_error.size())));

				// std::cout<<"rotation:"<<d_rotation<<",x_offset:"<<d_x_offset<<",y_offset:"<<d_y_offset<<",mean:"<<average_error<<",deviation:"<<deviation_error<<std::endl;

				if (average_error < min_error)
				{
					min_error = average_error;
					variance = deviation_error;

					best_d_rotation = d_rotation;
					best_d_x_offset = d_x_offset;
					best_d_y_offset = d_y_offset;
				}
			}

	// then we need to find a precise offset and rotation

	for (double d_rotation = best_d_rotation - 1.0; d_rotation <= best_d_rotation + 1.0; d_rotation = d_rotation + 0.05)
		for (double d_x_offset = best_d_x_offset - 40; d_x_offset <= best_d_x_offset + 40; d_x_offset = d_x_offset + 0.5)
			for (double d_y_offset = best_d_y_offset - 40; d_y_offset <= best_d_y_offset + 40; d_y_offset = d_y_offset + 0.5)
			{
				std::vector<double> v_error; // analysis of error

				for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); ++it_current_fingerprint)
				{
					// we find the ground truth

					it_ref_fingerprint = m_radio_fingerprints_optimized.find(it_current_fingerprint->first);

					if (it_ref_fingerprint != m_radio_fingerprints_optimized.end() && it_current_fingerprint->second.gt_found > 0)
					{
						carmen_point_t gt;
						gt.x = it_current_fingerprint->second.x;
						gt.y = it_current_fingerprint->second.y;

						carmen_point_t position;
						position.x = it_ref_fingerprint->second.x;
						position.y = it_ref_fingerprint->second.y;

						double e_x = position.x * cos(d_rotation) - position.y * sin(d_rotation) + d_x_offset + x_offset;
						double e_y = position.x * sin(d_rotation) + position.y * cos(d_rotation) + d_y_offset + y_offset;

						double error = sqrt((gt.x - e_x) * (gt.x - e_x) + (gt.y - e_y) * (gt.y - e_y));
						v_error.push_back(error);
					}
				}

				// caculate the mean and standard deviation of mean
				double sum_error = 0;
				double sum_deviation = 0;
				double average_error = 0;
				double deviation_error = 0;

				for (int i = 0; i < v_error.size(); i++)
				{
					sum_error = sum_error + v_error[i];
				}

				average_error = sum_error / ((double)(v_error.size()));

				for (int i = 0; i < v_error.size(); i++)
				{
					sum_deviation = sum_deviation + (v_error[i] - average_error) * (v_error[i] - average_error);
				}

				deviation_error = sqrt(sum_deviation / ((double)(v_error.size())));

				// std::cout<<"rotation:"<<d_rotation<<",x_offset:"<<d_x_offset<<",y_offset:"<<d_y_offset<<",mean:"<<average_error<<",deviation:"<<deviation_error<<std::endl;

				if (average_error < min_error)
				{
					min_error = average_error;
					variance = deviation_error;

					best_d_rotation = d_rotation;
					best_d_x_offset = d_x_offset;
					best_d_y_offset = d_y_offset;
				}
			}

	std::vector<carmen_point_t> v_ground_truth;

	std::vector<carmen_point_t> v_optimized;

	for (it_current_fingerprint = m_radio_fingerprints_extended.begin(); it_current_fingerprint != m_radio_fingerprints_extended.end(); ++it_current_fingerprint)
	{
		// we find the ground truth

		it_ref_fingerprint = m_radio_fingerprints_optimized.find(it_current_fingerprint->first);

		if (it_ref_fingerprint != m_radio_fingerprints_optimized.end() && it_current_fingerprint->second.gt_found > 0)
		{
			carmen_point_t gt;
			gt.x = it_current_fingerprint->second.x;
			gt.y = it_current_fingerprint->second.y;

			carmen_point_t position;
			position.x = it_ref_fingerprint->second.x;
			position.y = it_ref_fingerprint->second.y;

			carmen_point_t position_transform;

			double e_x = position.x * cos(best_d_rotation) - position.y * sin(best_d_rotation) + best_d_x_offset + x_offset;
			double e_y = position.x * sin(best_d_rotation) + position.y * cos(best_d_rotation) + best_d_y_offset + y_offset;

			position_transform.x = e_x;
			position_transform.y = e_y;
			v_ground_truth.push_back(gt);

			v_optimized.push_back(position_transform);
		}
	}

	plot_track_alignment(v_ground_truth, v_optimized);

	// close the database
	std::cout << "unique AP detected:"
			  << " " << m_mac_rss.size() << std::endl;
	std::cout << "dataset:" << m_fingerprints_file_name << ",sim threshold:" << m_sim_threshold << ",num_constrain:" << num_constrain << ",combined:" << combined_constrains << ",mean error:" << min_error << ",variance:" << variance << std::endl;
	std::cout << "x offset:" << best_d_x_offset << ",y offset:" << best_d_y_offset << ",angle offset:" << best_d_rotation << std::endl;

	// hold for plot
	 sleep(500000000000000000);//20 ms

	exit(0);
}
