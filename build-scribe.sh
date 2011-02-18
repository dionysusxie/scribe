scribedir=`dirname "$0"`
scribedir=`cd "$scribedir"; pwd`

if ! [ -e "$scribedir/build" ]; then
 mkdir -p $scribedir/build/include
 mkdir -p $scribedir/build/lib
fi


echo "Start building libhdfs..."

cd $HADOOP_HOME/src/c++/libhdfs
./configure JVM_ARCH=tune=generic --prefix=$scribedir/build
make && make install

cp $HADOOP_HOME/src/c++/libhdfs/hdfs.h $scribedir/build/include


echo "Start building scribe"
export CFLAGS="-I$JAVA_HOME/include/ -I$JAVA_HOME/include/linux/"
export CPPFLAGS=$CFLAGS
export LDFLAGS="-L$JAVA_HOME/jre/lib/amd64 -L$JAVA_HOME/jre/lib/amd64/server -L$scribedir/build/lib"

cd $scribedir
./bootstrap.sh --enable-hdfs --with-hadooppath=$scribedir/build --prefix=$scribedir/build
make && make install

