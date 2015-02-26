<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>

<!--
   $Id: trick_module.xsl 1375 2011-02-01 23:32:36Z wwoodard $

   Revision 7.2  2007-12-12 10:20:27-06  lin
   Add a new "REQUIREMENT" field to the Trick header

   Revision 7.1  2006-06-22 14:15:18-05  lin
   Bump version number for 07

   Revision 1.3  2006-05-15 09:32:29-05  lin
   Xml documeentation has some bugs

   Revision 1.2  2005-07-19 12:40:06-05  vetter
   XML Autogen Polishing

   Revision 1.1  2004-12-15 15:59:42-06  lin
   Initial revision
:
-->

<xsl:template match="/">
<style>
    td { border-style: inset; border-color: lightgray; border-width: 1px; }
    td.header  { border-style: inset; border-color: gray; border-width: 1px; }
</style>
<html>
    <head>
        <title><xsl:value-of select="/file/full_path_file_name"/></title>
    </head>
    <body>
    <font face="Arial">

        <a name="TOP"></a>
        <table bgcolor="#000000" width="100%" cellpadding="5" cellspacing="1">
            <tr>
                <td bgcolor="#fff2cc" >
                    <b><font size="5" face="Arial">
                        <xsl:value-of select="/file/file_name"/>
                    </font></b>
                </td>
            </tr>
        </table>

        <br></br><br></br>

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

                    <tr>
                        <td><TT>Purpose</TT></td>
                        <td><xsl:value-of select="purpose"/></td>
                    </tr>

                    <tr>
                        <td><TT>References</TT></td>
                        <td>
                            <table rules="all" bordercolor="lightgray" cellpadding="5">
                                <tr>
                                    <td width="15%"><TT>Author(s)</TT></td>
                                    <td width="25%"><TT>Source</TT></td>
                                    <td width="15%"><TT>Id</TT></td>
                                    <td width="15%"><TT>Location</TT></td>
                                    <td width="5%"><TT>Date</TT></td>
                                    <td width="15%"><TT>Notes</TT></td>
                                </tr>
                                <xsl:for-each select="references/reference">
                                    <tr>
                                        <td><xsl:value-of select="author"/></td>
                                        <td><xsl:value-of select="source"/></td>
                                        <td><xsl:value-of select="id"/></td>
                                        <td><xsl:value-of select="location"/></td>
                                        <td><xsl:value-of select="date"/></td>
                                        <td><xsl:value-of select="notes"/></td>
                                    </tr>
                                </xsl:for-each>
                            </table>
                        </td>
                    </tr>

                    <tr>
                        <td><TT>Assumptions</TT></td>
                        <td>
                                <table rules="all" bordercolor="lightgray" cellpadding="5">
                                    <xsl:for-each select="assumptions/assumption">
                                        <tr>
                                            <td>
                                                <xsl:value-of select="current()"/>
                                            </td>
                                        </tr>
                                    </xsl:for-each>
                                </table>
                        </td>
                    </tr>

                    <tr>
                        <td><TT>Requirements</TT></td>
                        <td>
                                <table rules="all" bordercolor="lightgray" cellpadding="5">
                                    <xsl:for-each select="requirements/requirement">
                                        <tr>
                                            <td>
                                                <xsl:value-of select="current()"/>
                                            </td>
                                        </tr>
                                    </xsl:for-each>
                                </table>
                        </td>
                    </tr>

                    <tr>
                        <td><TT>Job Class</TT></td>
                        <td><xsl:value-of select="class"/></td>
                    </tr>
                    <tr>
                        <td>
                            <TT>Library Dependencies</TT>
                        </td>
                        <td>
                            <table rules="all" bordercolor="lightgray" cellpadding="5">
                                <xsl:for-each select="library_dependencies/library_dependency">
                                    <tr>
                                        <td><xsl:value-of select="text()"/></td>
                                    </tr>
                                </xsl:for-each>
                            </table>
                        </td>
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
                                            <td><xsl:value-of select="notes"/></td>
                                        </tr>
                                    </xsl:for-each>
                                </table>
                            </td>
                        </tr>
                    </xsl:for-each>


                </table>
        </xsl:for-each>


        <br></br><br></br><br></br>

        <!-- Write out the jobs -->
        <xsl:for-each select="file/jobs/job">
        <table width="100%" cellpadding="15" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
            <tr>
                <td bgcolor="#ccccff" colspan="2" style="border-color: black; border-style: solid; border-size: 1px">
                    <b><font size="3">Argument And Return Types</font></b>
                </td>
            </tr>
            <tr>
                <td><TT>Return Information</TT></td>
                <td>
                    <table rules="all" bordercolor="lightgray" cellpadding="5">
                        <tr>
                            <td width="25%"><tt>Return Type</tt></td>
                            <td width="5%"><tt>Input/Output</tt></td>
                            <td width="5%"><tt>Units</tt></td>
                            <td width="65%"><tt>Description</tt></td>
                        </tr>
                        <tr>
                            <td><xsl:value-of select="return_type"/></td>
                            <td><xsl:value-of select="inout"/></td>
                            <td><xsl:value-of select="units"/></td>
                            <td><xsl:value-of select="comment"/></td>
                        </tr>
                    </table>
                </td>
            </tr>
            <tr>
                <td><TT>Arguments</TT></td>
                <td>
                    <table rules="all" bordercolor="lightgray" cellpadding="5">
                        <tr>
                            <td width="20%"><tt>Type</tt></td>
                            <td width="20%"><tt>Name</tt></td>
                            <td width="5%"><tt>Input/Output</tt></td>
                            <td width="5%"><tt>Units</tt></td>
                            <td width="50%"><tt>Description</tt></td>
                        </tr>
                        <xsl:for-each select="argument_list/arg">
                            <tr>
                                <td><xsl:value-of select="type"/><xsl:value-of select="dims"/></td>
                                <td><xsl:value-of select="arg_name"/></td>
                                <td><xsl:value-of select="inout"/></td>
                                <td><xsl:value-of select="units"/></td>
                                <td><xsl:value-of select="comment"/></td>
                            </tr>
                        </xsl:for-each>
                    </table>
                </td>
            </tr>
        </table>
        </xsl:for-each>

        <br></br><br></br><br></br>

        <!-- Write out the #includes -->
        <table width="100%" cellpadding="15" cellspacing="0" border="1" bordercolor="black" style="border-style: ridge;" >
            <tr>
                <td bgcolor="#ccccff" colspan="2" style="border-color: black; border-style: solid; border-size: 1px">
                    <b><font size="3">Included Headers</font></b>
                </td>
            </tr>

            <tr>
                <td>
                    <xsl:for-each select="file/includes/file">
                        <xsl:value-of select="current()"/><br></br>
                    </xsl:for-each>
                </td>
            </tr>
        </table>

        <br></br><br></br><br></br>

    </font>
    </body>
</html>

</xsl:template>
</xsl:stylesheet>

