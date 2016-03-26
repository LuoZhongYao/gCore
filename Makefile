.PHONY: lib relib build dump dfu goods flash
########################	工具设置 ##########################
BLUELAB			:= E:\\ADK4.0.0\\tools
XIPBUILD 		:= $(BLUELAB)\\..\\xide\\bin\\xipbuild.exe
BLUELAB_INCLUDE	:= $(BLUELAB)/include/
MAKE			:= $(BLUELAB)/bin/make.exe -R BLUELAB=$(BLUELAB) -j 4
SRC				:= src
SH				:= cmd /c
EDITOR 			:= vim -c 'set filetype=gitcommit'


#######################    helper function ####################
define to_hex
	echo -n $1|xxd -g 1 -i|tr -d ' '|sed 's/,\?0x/ 00/g'|sed 's/^\s*//'
endef

define lword_hex
	printf '%08x' $1|sed 's/\([0-9a-fA-F]\{4\}\)\([0-9a-fA-F]\{4\}\)/\1 \2/g'
endef

-include  $(SRC)/config.mk

ifeq ($(W),1)
dew 	 		:=
endif

goods			:= goods
makefile		:= aCTRL.release.mak
xip 			:= aCTRL.xip
time			:= $(shell date +%y-%m%d-%H)
id				?= GM
mode			?= assisted
chip 			?= gordon
name			?= GM8670
pin				?= 0000
rate			?= 115200
version 		?= $(shell $(call to_hex,$(id)$(shell date +%y%m%d%H)))
app_psr			?= app
app 			?= $(id)$(time)
dfu_suffix 		?= dfu
log_header 		?= $(app)
packet			:= $(id)$(time)
out				:= $(goods)/$(id)/$(packet)
dfu_tools		:= $(BLUELAB)/bin
unsigned_fw 	:= $(BLUELAB)/../firmware/$(mode)/unified/$(chip)
loader_unsigned := $(unsigned_fw)/loader_unsigned
stack_unsigned 	:= $(unsigned_fw)/stack_unsigned
dew 	 		?= "dew"

define up_psr
(\
	printf "// MODE 2 rate \n";\
	printf "&04B2 = 01ea\n";\
	printf "// Uart Rate\n";\
	printf "&0538 = $$($(call lword_hex,$(rate)))\n";\
	printf "// BT version\n";\
	printf "&025f = $(version)\n";\
	printf "// BT NAME\n";\
	printf "&2265 = $$($(call to_hex,$(name)))\n";\
	printf "// BT PIN \n";\
	printf "&2266 = $$($(call to_hex,$(pin)))\n";\
)
endef

define dump
	$(BLUELAB)/bin/BlueFlashCmd.exe -dump $(app) -usb 0
endef

define display
printf "\e[33m%s\e[0m\n" $1
endef

define dfu
	$(call display,"Generate Version $(version)") && \
    mkdir -p ../$(id) &&\
	echo -n "SBGM" > ../$(id)/random && 	\
	$(call display,"Generate Keys ......") 	&&\
	$(dfu_tools)/dfukeygenerate -o ../$(id)/stack_keys -r ../$(id)/random -i "stack_keys" &&\
	$(dfu_tools)/dfukeygenerate -o ../$(id)/app_keys -r ../$(id)/random -i "app_keys" &&\
	$(call display,"Insert Stack Public Keys To Loader ......") &&\
	$(dfu_tools)/dfukeyinsert 	-o ../$(id)/loader_singed -l $(loader_unsigned).xdv -ks ../$(id)/stack_keys.public.key &&\
	$(call display,"Insert Application Public Keys To stack.psr ......") &&\
	$(dfu_tools)/dfukeyinsert 	-o ../$(id)/ps_public -ps stack.psr -ka ../$(id)/app_keys.public.key &&\
	$(call display,"Sign Stack ......") &&\
	$(dfu_tools)/dfusign		-o ../$(id)/stack_singed  -s $(stacK_unsigned).xpv  -ks ../$(id)/stack_keys.private.key &&\
	$(call display,"Sign Application ......") &&\
	$(dfu_tools)/dfusign		-o ../$(id)/image_signed -h image.fs -ka ../$(id)/app_keys.private.key &&\
	$(call display,"Sign Protected stack.psr ......") &&\
	$(dfu_tools)/dfusign		-o ../$(id)/ps -ps ../$(id)/ps_public.psr -ks ../$(id)/stack_keys.private.key &&\
	$(call display,"Sign Protected $(app_psr).psr ......") &&\
	$(dfu_tools)/dfusign 		-o ../$(id)/ps -pa $(app_psr).psr -ka ../$(id)/app_keys.private.key &&\
	$(call display,"Build DFU ......") &&\
	$(dfu_tools)/dfubuild 		-pedantic -f $(app).$(dfu_suffix) -uv 0x0a12 -up 0x0001 -ui "BC5 MM (SBGM)"\
   	-h ../$(id)/image_signed.fs -p3 . ../$(id)/ps.stack.psr ../$(id)/ps.app.psr ;\
	echo -n $(dew) >> $(app).$(dfu_suffix);
endef

define update-bdaddr
	$(call display,"Update Bluetooth Addresss") &&\
	addr=$$(cat bdaddr.psr|awk '{print $$3}') && \
	bdaddr=$$(($$(echo -n "0x$$addr") + 1)) && \
	sed -i "$$(printf "s/=\s$$addr/= %04x/g" $$(echo -n $$bdaddr))" bdaddr.psr;
endef


#Operation Selection:
#     -b: Standard Build (default)
#     -f: Rebuild All
#     -c: Clean
#Configuration Selection:
#     -d: Build/clean default build configuration (default)
#     -a: Build/clean all build configurations
#     -n: Build/clean a named build configuration
#

defualt : build

build :
	${XIPBUILD} -b -d ${xip}

clean :
	${XIPBUILD} -c -d ${xip}

rebuild :
	${XIPBUILD} -f -d ${xip}

pin_name :
	$(call display,"App $(app)")
	$(call up_psr)

udr :
	$(call update-bdaddr)

flash : build
	@$(MAKE) -f $(makefile) $@
ifneq ($(update),)
	@$(call update-bdaddr)
	@$(BLUELAB)/bin/pscli -USB 0 merge bdaddr.psr
endif

psr :
	@$(call display,"id = $(id) rate = $(rate) pin = $(pin) name = $(name)")
	@mkdir -p $(out)
	@cp pskey/stack.psr pskey/$(app_psr).psr $(out)/
	@(printf "// MODE 2 rate \n";\
	printf "&04B2 = 01ea\n";\
	printf "// Uart Rate\n";\
	printf "&0538 = $$($(call lword_hex,$(rate)))\n";\
	printf "// BT version\n";\
	printf "&025f = $(version)\n";\
	printf "// BT PIN \n";\
	printf "&2266 = $$($(call to_hex,$(pin)))\n";) >> $(out)/stack.psr
ifneq ($(id),LC)
	@(printf "// BT NAME\n";\
	printf "&2265 = $$($(call to_hex,$(name)))\n") >> $(out)/stack.psr
endif

lib :
	@(cd lib/;$(MAKE) install)

relib :
	@(cd lib/;$(MAKE) clean)
	@(cd lib/;$(MAKE) install doxygen)

id : psr
	@$(BLUELAB)/bin/pscli -USB 0 merge $(out)/stack.psr
	@$(BLUELAB)/bin/pscli -USB 0 merge $(out)/$(app_psr).psr

dump : id
	@(cd $(out);$(call dump))

dfu : psr
	@cp $(SRC)/merge.x[dp]v	$(out)/
	@cp $(SRC)/image.fs $(out)/
	@(cd $(out);$(call dfu))

log :
	@printf "$(log_header) : %s\n" $$(date +%Y.%m.%d) >> $(goods)/change.txt
	@$(EDITOR) $(goods)/change.txt

goods : log dump dfu
