composite_loader: composite_loader.cpp
	mkdir -p build/
	c++ -O3 -Wall -Wextra -std=c++20 -o build/composite_loader composite_loader.cpp
composite_loader_with_parts: composite_loader_with_parts.cpp
	mkdir -p build/
	c++ -O3 -Wall -Wextra -std=c++20 -o build/composite_loader_with_parts composite_loader_with_parts.cpp
composite_loader17: composite_loader_cpp17.cpp
	mkdir -p build/
	c++ -O3 -Wall -Wextra -std=c++17 -o build/composite_loader17 composite_loader_cpp17.cpp
composite_loader_with_parts17: composite_loader_with_parts_cpp17.cpp
	mkdir -p build/
	c++ -O3 -Wall -Wextra -std=c++17 -o build/composite_loader_with_parts17 composite_loader_with_parts_cpp17.cpp
clean:
	rm -f build/composite_loader build/composite_loader_with_parts
	rm -f build/composite_loader17 build/composite_loader_with_parts17