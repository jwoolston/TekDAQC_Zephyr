zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ADC_ADS125X ads1256.c)