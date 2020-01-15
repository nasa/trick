

# Downloads the external java packages Trick uses.  We use the maven repository to
# get the jar file.  Currently we fix the version of the jar files we download.
# I wonder how long the links stay unchanged and active (Alex 1/22/15)

# Running in parallel works fine.  Not running in parallel makes the output messages
# readable.  I chose readability.
.NOTPARALLEL:

EXTERNAL_JARS = \
    bsaf-1.9.2.jar \
    jcommon-1.0.23.jar \
    jfreechart-1.0.19.jar \
    jfreesvg-2.1.jar \
    jopt-simple-4.8.jar \
    junit-4.12.jar \
    swingx-1.6.1.jar \
    hamcrest-core-1.3.jar \
    pdfbox-2.0.11.jar \
    fontbox-2.0.11.jar \
    commons-logging-1.2.jar \
    jaxb-runtime-2.4.0-b180830.0438.jar \
    jaxb-xjc-2.4.0-b180830.0438.jar \
    jaxb-jxc-2.4.0-b180830.0438.jar \
    jaxb-impl-2.4.0-b180830.0438.jar \
    jaxb-api-2.4.0-b180725.0427.jar \
    jaxb-core-2.3.0.1.jar \
    javax.activation-1.2.0.jar


external_jars : ${EXTERNAL_JARS}

bsaf-1.9.2.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/jdesktop/bsaf/bsaf/1.9.2/bsaf-1.9.2.jar
jcommon-1.0.23.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/jfree/jcommon/1.0.23/jcommon-1.0.23.jar
jfreechart-1.0.19.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/jfree/jfreechart/1.0.19/jfreechart-1.0.19.jar
jfreesvg-2.1.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/jfree/jfreesvg/2.1/jfreesvg-2.1.jar
jopt-simple-4.8.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/net/sf/jopt-simple/jopt-simple/4.8/jopt-simple-4.8.jar
junit-4.12.jar:
	curl --retry 4 -O https://repo1.maven.org/maven2/junit/junit/4.12/junit-4.12.jar
swingx-1.6.1.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/swinglabs/swingx/1.6.1/swingx-1.6.1.jar
hamcrest-core-1.3.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar
pdfbox-2.0.11.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/apache/pdfbox/pdfbox/2.0.11/pdfbox-2.0.11.jar
fontbox-2.0.11.jar :
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/apache/pdfbox/fontbox/2.0.11/fontbox-2.0.11.jar
commons-logging-1.2.jar :
	curl --retry 4 -O https://repo1.maven.org/maven2/commons-logging/commons-logging/1.2/commons-logging-1.2.jar
jaxb-runtime-2.4.0-b180830.0438.jar:	
	curl --retry 4 -O https://repo.maven.apache.org/maven2/org/glassfish/jaxb/jaxb-runtime/2.4.0-b180830.0438/jaxb-runtime-2.4.0-b180830.0438.jar
jaxb-xjc-2.4.0-b180830.0438.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-xjc/2.4.0-b180830.0438/jaxb-xjc-2.4.0-b180830.0438.jar
jaxb-jxc-2.4.0-b180830.0438.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-jxc/2.4.0-b180830.0438/jaxb-jxc-2.4.0-b180830.0438.jar
jaxb-impl-2.4.0-b180830.0438.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-impl/2.4.0-b180830.0438/jaxb-impl-2.4.0-b180830.0438.jar 
jaxb-core-2.3.0.1.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-core/2.3.0.1/jaxb-core-2.3.0.1.jar 
jaxb-api-2.4.0-b180725.0427.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/javax/xml/bind/jaxb-api/2.4.0-b180725.0427/jaxb-api-2.4.0-b180725.0427.jar
javax.activation-1.2.0.jar:
	curl --retry 4 -O https://repo.maven.apache.org/maven2/com/sun/activation/javax.activation/1.2.0/javax.activation-1.2.0.jar
clean:
	rm -f ${EXTERNAL_JARS}
