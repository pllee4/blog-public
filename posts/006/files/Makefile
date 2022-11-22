memory_leak: memory_leak.cpp
	mkdir -p build/
	c++ -O3 -Wall -Wextra -std=c++17 -fsanitize=address -o build/memory_leak memory_leak.cpp
	# c++ -O3 -Wall -Wextra -std=c++17 -o build/memory_leak memory_leak.cpp
clean:
	rm -f build/memory_leak