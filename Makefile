PIO := "C:/Users/nikita/.platformio/penv/Scripts/pio"

define make_env_targets
$(1):
	@$(PIO) run -e $(1) --target upload

$(1)_build:
	@$(PIO) run -e $(1)

$(1)_cfg:
	@$(PIO) run --target menuconfig
endef

# ---------- Environments ----------
$(eval $(call make_env_targets, controller))