CCOPTS = -fPIC -ggdb

all: lib  download_inputs download_outputs upload_inputs upload_outputs  card_config layer1_config layer2_config

lib: libCTP7AlgoClient.so

card_config: card_config.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

layer1_config: layer1_config.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

layer2_config: layer2_config.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

download_inputs: download_inputs.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

download_outputs: download_outputs.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

upload_inputs: upload_inputs.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

upload_outputs: upload_outputs.cpp libCTP7AlgoClient.so CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h
	g++ $(CCOPTS) -o $@ $< -I. -L. -lCTP7AlgoClient -Irpcsvc_client_dev -Lrpcsvc_client_dev -lwiscrpcsvc

libCTP7AlgoClient.so: CTP7AlgoClient.o tinyxml2.o
	g++ $(CCOPTS) -shared -o $@ $^ -Lrpcsvc_client_dev -lwiscrpcsvc

CTP7AlgoClient.o: CTP7AlgoClient.cpp CTP7AlgoClient.hh rpcsvc_client_dev/wiscrpcsvc.h tinyxml2.h
	g++ $(CCOPTS) -c -o $@ $< -Irpcsvc_client_dev

tinyxml2.o: tinyxml2.cpp tinyxml2.h
	g++ $(CCOPTS) -c -o $@ $< -fvisibility=hidden

rpcsvc_client_dev/wiscrpcsvc.h: client_dev.tbz2
	@rm -rf rpcsvc_client_dev/
	tar -xjf $<
	$(MAKE) -C rpcsvc_client_dev/ lib
	@touch $@

client_dev.tbz2:
	$(error Please place the rpcsvc client_dev.tbz2 package in this directory before running make)

.SECONDARY: client_dev.tbz2

clean:
	rm -rf rpcsvc_client_dev/ layer1_config layer2_config card_config download_inputs download_outputs upload_inputs testapp *.so *.o

distclean: clean
	rm -f client_dev.tbz2

.PHONY: all clean distclean
