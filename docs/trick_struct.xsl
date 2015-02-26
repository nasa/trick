<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<!--
   $Id: trick_struct.xsl 1375 2011-02-01 23:32:36Z wwoodard $

   Revision 7.2  2008-04-14 14:04:28-05  hchen
   The memeber columns are empty when the auto-generated header xml is viewed

   Revision 7.1  2006-06-22 14:15:19-05  lin
   Bump version number for 07

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
        <title><xsl:value-of select="file/full_path_file_name"/></title>
    </head>
    <body>
    <font face="Arial">
        <!-- write out the trick header -->
        <xsl:for-each select="file/trick_header">
                <table width="100%" cellpadding="15" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
                    <tr>
                        <td bgcolor="#ccccff" colspan="2" style="border-color: black; border-style: solid; border-size: 1px">
                            <b><font size="3"> Trick Header Information</font></b>
                        </td>
                    </tr>
                    <tr>
                        <td>
                            <TT>File</TT>
                        </td>
                        <td>
                             <a>
                             <xsl:attribute name="href">
                             <xsl:value-of select="/file/full_path_file_name"/>
                             </xsl:attribute>
                             <xsl:value-of select="/file/file_name"/>
                             </a>
                        </td>
                    </tr>
                    <xsl:for-each select="reference">
                        <tr>
                            <td><TT>Reference</TT></td>
                            <td>
                                <table rules="all" bordercolor="lightgray" cellpadding="5">
                                    <tr>
                                        <td width="15%"><TT>Author(s)</TT></td>
                                        <td width="25%"><TT>Source</TT></td>
                                        <td width="15%"><TT>Id</TT></td>
                                        <td width="15%"><TT>Location</TT></td>
                                        <td width="5%"><TT>Date</TT></td>
                                        <td width="25%"><TT>Notes</TT></td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <xsl:for-each select="authors/author">
                                                <xsl:value-of select="current()"/><br></br>
                                            </xsl:for-each>
                                        </td>
                                        <td><xsl:value-of select="source"/></td>
                                        <td><xsl:value-of select="id"/></td>
                                        <td><xsl:value-of select="location"/></td>
                                        <td><xsl:value-of select="date"/></td>
                                        <td><xsl:value-of select="notes"/></td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                    </xsl:for-each>
                    <xsl:for-each select="assumption">
                        <tr>
                            <td><TT>Assumption</TT></td>
                            <td><xsl:value-of select="current()"/></td>
                        </tr>
                    </xsl:for-each>
                    <tr>
                        <td><TT>Purpose</TT></td>
                        <td><xsl:value-of select="purpose"/></td>
                    </tr>
                    <xsl:for-each select="modifications">
                        <tr>
                            <td><TT>Modifications</TT></td>
                            <td>
                                <table rules="all" bordercolor="lightgray" cellpadding="5">
                                    <tr>
                                        <td width="15%"><TT>Programmer</TT></td>
                                        <td width="15%"><TT>Employer</TT></td>
                                        <td width="10%"><TT>Date</TT></td>
                                        <td width="15%"><TT>Id</TT></td>
                                        <td width="45%"><TT>Description</TT></td>
                                    </tr>
                                    <xsl:for-each select="modification">
                                        <tr>
                                            <td> <xsl:value-of select="programmer"/></td>
                                            <td> <xsl:value-of select="employer"/></td>
                                            <td><xsl:value-of select="date"/></td>
                                            <td><xsl:value-of select="id"/></td>
                                            <td><xsl:value-of select="description"/></td>
                                        </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </xsl:for-each>
                </table>
        </xsl:for-each>

        <br></br><br></br><br></br>

        <!-- Write out the enumerated types -->
        <table width="100%" cellpadding="15" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
            <tr>
                <td bgcolor="#ccccff" colspan="2" style="border-color: black; border-style: solid; border-size: 1px">
                    <b><font size="3"> Enumerations </font></b>
                </td>
            </tr>
            <xsl:for-each select="file/enumerations/enum">
                <tr>
                    <td style="border-style: none">
                        <table bgcolor="#fff2cc" bordercolor="black" cellpadding="5"
                               style="border-style: solid;
                                  -moz-border-radius: 12px 12px 0px 0px;
                                  border-width: 1px 1px 0px 1px;">
                            <tr>
                                <a>
                                    <xsl:attribute name="name">
                                        <xsl:value-of select="enum_name"/>
                                    </xsl:attribute>
                                </a>
                                <td style="border-style: none">
                                <b><xsl:value-of select="enum_name"/></b>
                                </td>
                            </tr>
                        </table>
                        <table bordercolor="lightgray" cellpadding="5" style="border-style: none; border-collapse: collapse;">
                            <tr>
                                <td class="header" width="25%">Member</td>
                                <td class="header" width="5%">Value</td>
                                <td class="header" width="70%">Description</td>
                            </tr>
                            <xsl:for-each select="member">
                                <tr>
                                    <td><xsl:value-of select="mname"/></td>
                                    <td><xsl:value-of select="value"/></td>
                                    <td><xsl:value-of select="comment"/></td>
                                </tr>
                            </xsl:for-each>
                        </table>
                    </td>
                </tr>
            </xsl:for-each>
        </table>

        <br></br><br></br><br></br>

        <!-- Write out the struct types -->
        <table width="100%" cellpadding="15" cellspacing="0" border="2" bordercolor="black"
               style="border-style: ridge;">
            <tr>
                <td bgcolor="#ccccff" colspan="2" style="border-color: black; border-style: solid; border-size: 1px;">
                    <b><font size="3"> Structures </font></b>
                </td>
            </tr>
            <xsl:for-each select="file/structures/struct">
                <tr>
                    <a>
                        <xsl:attribute name="name">
                            <xsl:value-of select="struct_name"/>
                        </xsl:attribute>
                    </a>
                    <td style="border-style: none">
                        <table bgcolor="#fff2cc" bordercolor="black" cellpadding="5"
                               style="border-style: solid;
                                  -moz-border-radius: 12px 12px 0px 0px;
                                  border-width: 1px 1px 0px 1px;">
                            <tr>
                                <td style="border-style: none">
                                    <b><xsl:value-of select="struct_name"/></b>
                                </td>
                            </tr>
                        </table>
                        <table bordercolor="lightgray" cellpadding="5" style="border-style: none; border-collapse: collapse;">
                            <tr>
                                <td class="header" width="20%">Member</td>
                                <td class="header" width="20%">Type</td>
                                <td class="header" width="5%">Dims</td>
                                <td class="header" width="5%">Units</td>
                                <td class="header" width="50%">Description</td>
                            </tr>
                            <xsl:for-each select="member">
                                <tr>
                                    <td> <xsl:value-of select="pname"/> </td>
                                     <xsl:if test="link">
                                        <td>
                                            <a>
                                                <xsl:attribute name="href">
                                                    <xsl:value-of select="link"/>#<xsl:value-of select="type"/>
                                                </xsl:attribute>
                                                <xsl:value-of select="type"/>
                                                <xsl:if test="is_enum">
                                                      <sup>e</sup>
                                                </xsl:if>
                                            </a>
                                        </td>
                                    </xsl:if>
                                    <xsl:if test="not(link)">
                                        <td><xsl:value-of select="type"/></td>
                                    </xsl:if>

                                    <td><xsl:value-of select="dims"/></td>
                                    <td><xsl:value-of select="units"/></td>
                                    <td><xsl:value-of select="comment"/></td>
                                </tr>
                            </xsl:for-each>
                        </table>
                    </td>
                </tr>
            </xsl:for-each>
        </table>
    </font>
    </body>
</html>

</xsl:template>
</xsl:stylesheet>

