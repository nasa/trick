<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<!--
   $Id: trick_s_define.xsl 1375 2011-02-01 23:32:36Z wwoodard $

   Revision 1.2  2005-07-19 12:40:07-05  vetter
   XML Autogen Polishing

   Revision 1.1  2004-12-15 16:03:30-06  lin
   Initial revision

-->

<xsl:template match="/">
<style>
    td { border-style: inset; border-color: lightgray; border-width: 1px; }
    td.header  { border-style: inset; border-color: gray; border-width: 1px; }
</style>
<html>
    <head>
        <title><xsl:value-of select="s_define/title"/></title>
    </head>

    <body>
    <font face="Arial">

        <a name="TOP"></a>
        <table bgcolor="#000000" width="100%" cellpadding="5" cellspacing="1">
            <tr>
                <td bgcolor="#fff2cc" >
                    <b><font size="5" face="Arial">
                        <xsl:value-of select="s_define/title"/>
                    </font></b>
                </td>
            </tr>
        </table>

        <br></br><br></br>

        <!-- write out index to sim_objects -->
        <table width="100%" cellpadding="5" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
            <tr>
                <td bgcolor="#ccccff">
                    <b><font size="3" face="Arial">
                        Simulation Objects
                    </font></b>
                </td>
            </tr>
            <tr>
                <td>
                    <xsl:for-each select="s_define/sim_object">
                                <a>
                                    <xsl:attribute name="href">
                                        #<xsl:value-of select="sim_object_name"/>
                                    </xsl:attribute>
                                    <xsl:value-of select="sim_object_name"/>
                                </a>
                                <br></br>
                    </xsl:for-each>
                </td>
            </tr>
        </table>

        <br></br><br></br>

        <!-- write out each sim_objects -->
        <xsl:for-each select="s_define/sim_object">
            <table width="100%" cellpadding="5" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
                <tr>
                    <td bgcolor="#ccccff" colspan="2">
                        <b><font size="3" face="Arial">
                            <a>
                                <xsl:attribute name="name">
                                    <xsl:value-of select="sim_object_name"/>
                                </xsl:attribute>
                                <xsl:value-of select="sim_object_name"/>
                                object
                            </a>
                        </font></b>
                    </td>
                </tr>
                <tr>
                    <td>
                        <TT>Structures &amp; Initialization Data</TT>
                    </td>
                    <td>
                        <xsl:for-each select="structs/struct">
                            <a>
                                <xsl:attribute name="href">
                                    <xsl:value-of select="link"/>#<xsl:value-of select="struct_name"/>
                                </xsl:attribute>
                                <xsl:value-of select="struct_name"/>
                            </a>
                            <br></br>
                            <xsl:for-each select="default_data">
                                &nbsp; &nbsp; &nbsp; &nbsp;
                                <a>
                                    <xsl:attribute name="href">
                                        <xsl:value-of select="file"/>
                                    </xsl:attribute>
                                    <xsl:value-of select="file"/>
                                </a>
                                <br></br>
                            </xsl:for-each>
                        </xsl:for-each>
                    </td>
                </tr>
                <tr>
                    <td><TT>Jobs</TT></td>
                    <td>
                        <xsl:for-each select="jobs/job">
                            <a>
                                <xsl:attribute name="href">
                                    <xsl:value-of select="link"/>
                                </xsl:attribute>
                                <xsl:value-of select="job_name"/>
                                ()
                            </a>
                            <br></br>
                        </xsl:for-each>
                    </td>
                </tr>
            </table>
            <br></br><br></br>
        </xsl:for-each>

        Generated from <a><xsl:attribute name="href">S_define</xsl:attribute>S_define</a>
        <br></br>

    </font>
    </body>
</html>

</xsl:template>
</xsl:stylesheet>

