IF (Qt5Mqtt_FOUND)

add_executable (mqttunittest MQTTUnitTest.cpp)

target_link_libraries(mqttunittest Qt5::Test)
target_link_libraries(mqttunittest KF5::Archive KF5::XmlGui ${GSL_LIBRARIES} ${GSL_CBLAS_LIBRARIES})

IF (Qt5SerialPort_FOUND)
	target_link_libraries(mqttunittest Qt5::SerialPort )
ENDIF ()
IF (KF5SyntaxHighlighting_FOUND)
	target_link_libraries(mqttunittest KF5::SyntaxHighlighting )
ENDIF ()
#TODO: KF5::NewStuff

IF (Cantor_FOUND)
	target_link_libraries(mqttunittest Cantor::cantorlibs )
ENDIF ()
IF (HDF5_FOUND)
	target_link_libraries(mqttunittest ${HDF5_C_LIBRARIES} )
ENDIF ()
IF (FFTW3_FOUND)
	target_link_libraries(mqttunittest ${FFTW3_LIBRARIES} )
ENDIF ()
IF (netCDF_FOUND)
	target_link_libraries(mqttunittest ${netCDF_LIBRARIES} )
ENDIF ()
IF (CFITSIO_FOUND)
	target_link_libraries(mqttunittest ${CFITSIO_LIBRARIES} )
ENDIF ()
IF (USE_LIBORIGIN)
	target_link_libraries(mqttunittest liborigin-static )
ENDIF ()

target_link_libraries(mqttunittest Qt5::Mqtt )
target_link_libraries(mqttunittest labplot2lib)

add_test(NAME mqttunittest COMMAND mqttunittest)

ENDIF ()
