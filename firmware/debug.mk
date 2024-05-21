.PHONY: $(BINARY).openocd
$(BINARY).openocd:
	@echo source [find interface/stlink-dap.cfg] > $(BINARY).openocd
	@echo set WORKAREASIZE 0x8000 >> $(BINARY).openocd
	@echo transport select \"dapdirect_swd\" >> $(BINARY).openocd
	@echo set CHIPNAME $(CHIPNAME) >> $(BINARY).openocd
	@echo set ENABLE_LOW_POWER 1 >> $(BINARY).openocd
	@echo set STOP_WATCHDOG 1 >> $(BINARY).openocd
	@echo set CLOCK_FREQ 8000 >> $(BINARY).openocd
	@echo reset_config srst_only srst_nogate connect_assert_srst >> $(BINARY).openocd
	@echo set CONNECT_UNDER_RESET 1 >> $(BINARY).openocd
	@echo set CORE_RESET 0 >> $(BINARY).openocd
	@echo set AP_NUM 0 >> $(BINARY).openocd
	@echo gdb_port $(GDB_PORT) >> $(BINARY).openocd
	@echo source [find $(TARGETCFG)] >> $(BINARY).openocd
