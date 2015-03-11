all: list safe_type

list: list.cpp
	g++ list.cpp -I${QTDIR}/include/ -L${QTDIR}/lib/ -lQtCore -o $@


safe_type: safe_type.cpp
	g++ safe_type.cpp -I../../include/ -o $@

clean:
	$(RM) safe_type list