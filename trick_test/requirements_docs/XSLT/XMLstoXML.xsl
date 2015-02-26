<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" version="1.0" indent="yes" encoding="UTF-8"/>

<xsl:template match="/">
	<testresults>
		<xsl:text>&#xa;</xsl:text>
		<xsl:for-each select="test_list/file">
			<xsl:apply-templates select="document(@name)/testsuites/testsuite"/>
		</xsl:for-each>
	</testresults>
</xsl:template>

<xsl:template match="testsuite">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" />
	</xsl:copy>
</xsl:template>

<xsl:template match="@*|node()">
	<xsl:copy>
	    <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>

