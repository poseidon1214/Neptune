# 若缺少头文件或无lib/libspa_text_miner.so，请先运行./dist-pkg.sh inc

CXX_INC=-I./include -I./include/thirdparty
CXX_LIB=-L./lib -lspa_text_miner

text_miner_demo_main:text_miner_demo_main.o
	g++ -o $@ text_miner_demo_main.o $(CXX_LIB)

%.o:%.cc
	g++ -c $< -o $@ $(CXX_INC)

.PHONY:
clean:
	rm -f text_miner_demo_main *.o
