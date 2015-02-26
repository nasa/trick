all: build_date
	@ xsltproc XSLT/XMLstoXML.xsl src_xml/Test_List.xml > src_xml/AllTests.xml
	@ $(MAKE) -C TagRequirements/
	@ ./TagRequirements/TagRequirements src_xml/TopLevelReqs.xml > src_xml/TopLevelReqs_tag.xml
	@ xsltproc XSLT/REQtoREQNUM.xsl src_xml/TopLevelReqs_tag.xml > src_xml/intermediate.xml
	@ xsltproc XSLT/XMLtoHTML.xsl src_xml/intermediate.xml > Requirements.html
	@ rm -rf build_date.xml
	@ rm -rf src_xml/intermediate.xml
	@ rm -rf src_xml/TopLevelReqs_tag.xml
	@ rm -rf src_xml/AllTests.xml

build_date:
	@ echo "<build_date date=\"`date`\"/>" > build_date.xml

clean:
	@ rm -rf Requirements.html
	@ $(MAKE) -C TagRequirements clean

install: all
	@ cp Requirements.html ${TRICK_HOME}/docs/.
