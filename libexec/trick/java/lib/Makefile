
# Downloads the external java packages Trick uses.  We use the maven repository to
# get the jar file.  Currently we fix the version of the jar files we download.
# I wonder how long the links stay unchanged and active (Alex 1/22/15)

# Running in parallel works fine.  Not running in parallel makes the output messages
# readable.  I chose readability.
.NOTPARALLEL:

EXTERNAL_JARS = \
    bsaf-1.9.2.jar \
    itext-2.1.7.jar \
    jcommon-1.0.23.jar \
    jfreechart-1.0.19.jar \
    jfreesvg-2.1.jar \
    jopt-simple-4.8.jar \
    junit-4.12.jar \
    swingx-1.6.1.jar \
    hamcrest-core-1.3.jar

external_jars : ${EXTERNAL_JARS}

bsaf-1.9.2.jar :
	curl -O http://repo.maven.apache.org/maven2/org/jdesktop/bsaf/bsaf/1.9.2/bsaf-1.9.2.jar
itext-2.1.7.jar :
	curl -O http://repo.maven.apache.org/maven2/com/lowagie/itext/2.1.7/itext-2.1.7.jar
jcommon-1.0.23.jar :
	curl -O http://repo.maven.apache.org/maven2/org/jfree/jcommon/1.0.23/jcommon-1.0.23.jar
jfreechart-1.0.19.jar :
	curl -O http://repo.maven.apache.org/maven2/org/jfree/jfreechart/1.0.19/jfreechart-1.0.19.jar
jfreesvg-2.1.jar :
	curl -O http://repo.maven.apache.org/maven2/org/jfree/jfreesvg/2.1/jfreesvg-2.1.jar
jopt-simple-4.8.jar :
	curl -O http://repo.maven.apache.org/maven2/net/sf/jopt-simple/jopt-simple/4.8/jopt-simple-4.8.jar
junit-4.12.jar:
	curl -O http://repo1.maven.org/maven2/junit/junit/4.12/junit-4.12.jar
swingx-1.6.1.jar :
	curl -O http://repo.maven.apache.org/maven2/org/swinglabs/swingx/1.6.1/swingx-1.6.1.jar
hamcrest-core-1.3.jar :
	curl -O http://repo.maven.apache.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar

clean:
	rm -f ${EXTERNAL_JARS}
