# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./dem.h \
    ./dem1.h \
    ./parameters.h \
    ./mlibutil/mlibutil.h \
    ./gui/geometryengine.h \
    ./gui/mainwindow.h \
    ./gui/mainwidget.h \
    ./ceres/ceres_initialization.h \
    ./ceres/ceres_motion.h \
    ./ceres/ceres_param.h \
    ./ceres/ceres_track.h \
    ./dlib/dlib_face_detector.h \
    ./reader/image_reader.h \
    ./reader/image_reader_kinect.h \
    ./reader/model_reader.h \
    ./Eigen/eigen_binary_io.h
SOURCES += ./dem.cpp \
    ./dem1.cpp \
    ./main.cpp \
    ./parameters.cpp \
    ./mlibutil/mlibutil.cpp \
    ./gui/geometryengine.cpp \
    ./gui/mainwidget.cpp \
    ./gui/mainwindow.cpp \
    ./ceres/ceres_initialization.cpp \
    ./ceres/ceres_motion.cpp \
    ./ceres/ceres_param.cpp \
    ./ceres/ceres_track.cpp \
    ../../../thirdparty2015/dlib/dlib/all/source.cpp
RESOURCES += gui/shaders.qrc
