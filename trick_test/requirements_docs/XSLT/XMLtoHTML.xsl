<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html"/>

<xsl:template match="/">
    <html style="font-family:arial;">
    <head>
    	<style type="text/css">
    		h3 {
    			line-height:110%;
    			margin-bottom:1}
			p {
    			line-height:110%;
    			margin:10;
    			margin-bottom:0} 	
    		p.unitTest {
    			line-height:110%;
    			margin:2;
    			font-family:courier;
    			font-size:14px"}
    	</style>
    </head>
	<body>

	<h1>Trick Software Requirements Specification</h1>
	<h2 style="display:inline">Automation, Robotics and Simulation Division</h2>
	<h3 style="text-align:right;margin:2">Trick 13&#160;&#160;</h3>
	<p style="text-align:right;font-size:12px"><b>Last updated: </b> 
		<xsl:apply-templates select="document('../build_date.xml')/build_date"/></p>
	<hr/>
	<p style="font-family:courier">Total Tests: 
		<b><xsl:value-of 
		select="count(document('../src_xml/AllTests.xml')/testresults/testsuite/testcase)"/></b>
		&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;Failures:
		<b><xsl:value-of 
		select="count(document('../src_xml/AllTests.xml')/testresults/testsuite/testcase/failure)"/></b>
	</p>
	<br/>

	<xsl:apply-templates select="document('../src_xml/headerReqs.xml')/introDocs/intro"/>
	<br/><br/><br/>
	<xsl:apply-templates select="requirement"/>
	
	</body>
	</html>
</xsl:template>

<xsl:template match="build_date">
	<xsl:value-of select="@date"/>
</xsl:template>

<xsl:template match="intro">
	<h3 style="margin-left:{(count(ancestor::*)-1)*.4}in">
		<b><xsl:value-of select="@num"/></b>
		<xsl:text>&#160;&#160;&#160;&#160;</xsl:text>
		<xsl:value-of select="@title"/></h3>
	<xsl:if test="@statement">
		<p style="margin-left:{(count(ancestor::*)-1)*.4}in;">
			<xsl:value-of select="@statement"/></p>
	</xsl:if>
	<xsl:if test="@note">
	     <p style="margin-left:{(count(ancestor::*)-1)*.4}in;">
	        <xsl:value-of select="@note"/></p>
	</xsl:if>

	<xsl:apply-templates select="intro"/>
</xsl:template>

<xsl:template match="requirement">
	<xsl:param name="tag" select="@tag"/>

	<xsl:variable name="fred" select="count(ancestor::*)"/>
	<xsl:variable name="tab" select="$fred * 30"/>

	<xsl:variable name="apple" 
		select="concat(../../../../../../../@numbr, '.' ,../../../../../../@numbr, 
				'.', ../../../../../@numbr, '.', ../../../../@numbr, '.', 
				../../../@numbr, '.', ../../@numbr, '.', ../@numbr, 
				'.', @numbr)"/>

	<xsl:variable name="numbering" select="concat(3,substring-after($apple,'1'))"/>
	<xsl:variable name="txt_ind" select="string-length($numbering)*10"/>

	<xsl:choose>
	<xsl:when test="contains(@statement,'[HEADER]')">
	<h3 style="margin-left:{$tab}px">
		<span style="font-weight:bold;">
		<xsl:value-of select="$numbering"/></span>
		<xsl:text>&#160;&#160;&#160;&#160;</xsl:text>
		<xsl:value-of select="normalize-space(substring-after(@statement,'[HEADER]'))"/>
	</h3>
	<xsl:if test="@note">
		<p style="margin-left:{$tab}px">
			<xsl:value-of select="@note"/></p>
	</xsl:if>		
	</xsl:when>

	<xsl:otherwise>
		<p style="margin-left:{$tab+$txt_ind}px; text-indent:-{$txt_ind}px;">

		<b><xsl:value-of select="$numbering"/></b>
		<xsl:text>&#160;&#160;&#160;&#160;&#160;&#160;</xsl:text>
		<xsl:value-of select="@statement"/>
		<span style="font-size:10pt;">
		<font color="gray">
		<xsl:text>&#160;&#160;&#160;[</xsl:text>
		<xsl:value-of select="$tag"/>]<br/></font></span>
		<xsl:if test="@note">
			<i><b>NOTE:&#160;</b><xsl:value-of select="@note"/></i>
		</xsl:if>
	</p>
	</xsl:otherwise>
	</xsl:choose>

	<xsl:choose>
	<xsl:when test="@verify">
		<p class="unitTest" 
	    	style="margin-left:{($fred)*30+$txt_ind}px;">
	        <font color="blue"><b>[VERIFIED] </b></font>
	        <xsl:value-of select="@verify"/>
	    </p>
	</xsl:when>

	<xsl:otherwise>
		<xsl:for-each select="document('../src_xml/AllTests.xml')/testresults/testsuite">
			<xsl:for-each select="testcase">
				<xsl:if test="@parent">
					<p class="unitTest" 
						style="margin-left:{($fred)*30+$txt_ind}px;">

						<xsl:call-template name="splitString">
							<xsl:with-param name="tag1" select="$tag"/>
							<xsl:with-param name="parNums" select="translate(@parent,',',' ')"/>
						</xsl:call-template>
					</p>
				</xsl:if>
	 		</xsl:for-each>
		</xsl:for-each>
	</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="requirement"/>
</xsl:template>

<xsl:template name="splitString">
	<xsl:param name="tag1"/>
	<xsl:param name="parNums" select="."/>
	
	<xsl:if test="string-length($parNums)">
		<xsl:variable name="split"
			select="normalize-space(substring-before(concat($parNums,' '),' '))"/> 
		<xsl:choose>
		<xsl:when test="$tag1 = $split">
			<xsl:call-template name="printResults"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="splitString">
				<xsl:with-param name="tag1" select="$tag1"/>
				<xsl:with-param name="parNums" select="substring-after($parNums,' ')"/>
			</xsl:call-template>
		</xsl:otherwise>
		</xsl:choose>
	</xsl:if>
</xsl:template>

<xsl:template name="printResults">
	<xsl:param name="counter"/>
	<xsl:choose>
	<xsl:when test="failure"> 
		<font color="red"><b>[ FAILED ] </b></font>
	</xsl:when>
	<xsl:otherwise> 
		<font color="green"><b>[ PASSED ] </b></font>
	</xsl:otherwise>
	</xsl:choose>
	<xsl:value-of select="@classname"/>
	<xsl:text>:&#160;</xsl:text>
	<xsl:value-of select="@name"/>
</xsl:template>

</xsl:stylesheet>
