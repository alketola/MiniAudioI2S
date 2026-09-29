idf_component_register(
    SRCS "src/MiniAudioI2S.cpp"
    INCLUDE_DIRS "src"
    PRIV_REQUIRES i2s log  # Specify ESP-IDF drivers you need
)
