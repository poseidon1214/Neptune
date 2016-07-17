1. 本目录包含了TextMiner所有依赖的第三方库，其中头文件在include中，实现则统一打包
   在lib/libspa_text_miner.so中。
2. text_miner_demo_main.cc演示了如何利用上述so库来使用TextMiner, 编译设置参见Makefile
3. 如何运行请参见run_text_miner_demo_main.sh，由于历史原因，参数--segmenter_data_dir
   必须如此显式指定。
4. data目录下数据和模型文件请找fandywang
5. 当第三方应用内嵌此so里应注意:
   a) 若第三方应用也使用了PB，必须使用protobuf-2.6.1, 否则可能编译时提示版本不兼容。
   b) 分词参数--segmenter_data_dir应该在初始化text_miner之前按以下方式指定:
      int argc = 2;
      char* argv_tmp[] = {"spa_text_miner", "--segmenter_data_dir=./data/tc_data/"};
      char** argv = &(argv_tmp[0]);
      ::google::ParseCommandLineFlags(&argc, &argv, true);
      ::google::InitGoogleLogging(argv[0]);
