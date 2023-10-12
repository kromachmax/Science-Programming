
all: exe lib

exe: ./plugins/sin.h ./plugins/cos.h ./plugins/degree.h ./plugins/ln.h ./plugins/exp.h ./plugins/exp.cpp calculator.cpp lib
	g++ calculator.cpp -fPIC -ldl -o calculator

lib: ./plugins/sin.h ./plugins/sin.cpp ./plugins/cos.h	./plugins/cos.cpp ./plugins/degree.h ./plugins/degree.cpp ./plugins/ln.h ./plugins/ln.cpp ./plugins/exp.h ./plugins/exp.cpp
	 g++ -shared ./plugins/sin.cpp -fPIC -o ./plugins/libFuncSin.so
	 g++ -shared ./plugins/cos.cpp -fPIC -o ./plugins/libFuncCos.so 
	 g++ -shared ./plugins/degree.cpp -fPIC -o ./plugins/libFuncDegree.so 
	 g++ -shared ./plugins/ln.cpp -fPIC -o ./plugins/libFuncLn.so
	 g++ -shared ./plugins/exp.cpp -fPIC -o ./plugins/libFuncExp.so
clean: 
	 -rm calculator ./plugins/libFuncSin.so 2>/dev/null
	 -rm ./plugins/libFuncCos.so 2>/dev/null
	 -rm ./plugins/libFuncDegree.so 2>/dev/null
	 -rm ./plugins/libFuncLn.so 2>/dev/null
	 -rm ./plugins/libFuncExp.so 2>/dev/null

 
	 
