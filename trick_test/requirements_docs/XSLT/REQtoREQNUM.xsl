<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" version="1.0" indent="yes" encoding="UTF-8"/>

<xsl:template match="/">
	<xsl:apply-templates match="requirement"/>
</xsl:template>

<xsl:template match="requirement">
	<xsl:copy>
		<xsl:attribute name="numbr">
			<xsl:number/>
		</xsl:attribute>
		<xsl:apply-templates select="@*|node()" />
	</xsl:copy>
</xsl:template>

<xsl:template match="@*|node()">
	<xsl:copy>
    	<xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>

