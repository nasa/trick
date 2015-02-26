<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>

<!--
   $Id: trick_module_tex.xsl 1375 2011-02-01 23:32:36Z wwoodard $

   Revision 7.2  2007-12-12 10:20:28-06  lin
   Add a new "REQUIREMENT" field to the Trick header

   Revision 7.1  2006-06-22 14:15:30-05  lin
   Bump version number for 07

   Revision 1.1  2005-10-07 10:15:15-05  lin
   Initial revision

-->

<xsl:template match="/file">
    <xsl:apply-templates select="file_name"/>
    <xsl:apply-templates select="trick_header"/>
    <xsl:if test="count(includes) != 0">
{\bf Include Files:}
\begin{itemize} <xsl:for-each select="includes">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="file"/></xsl:call-template>
        </xsl:for-each>
\end{itemize}
    </xsl:if>

{\bf Functions:}
The following functions are defined in {\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="file_name"/></xsl:call-template> }
\begin{itemize} <xsl:for-each select="jobs/job">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="job_name"/></xsl:call-template>
    </xsl:for-each>
\end{itemize}
\vspace{0.125in}
    <xsl:apply-templates select="jobs"/>
</xsl:template>

<xsl:template match="file_name">
{\bf File:} {\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="text()"/></xsl:call-template> }
\vspace{0.125in}
</xsl:template>

<!-- write out the trick header -->
<xsl:template match="trick_header">
    <xsl:apply-templates select="purpose"/>
    <xsl:apply-templates select="references"/>
    <xsl:apply-templates select="assumptions"/>
    <xsl:apply-templates select="library_dependencies"/>
</xsl:template>

<xsl:template match="purpose">
{\bf Purpose:}
    <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="text()"/></xsl:call-template>
\vspace{0.125in}
</xsl:template>

<xsl:template match="references">
{\bf References:}
    <xsl:if test="count(reference) != 0">
\begin{itemize} <xsl:apply-templates select="reference"/>
\end{itemize}
    </xsl:if>
</xsl:template>

<xsl:template match="reference">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="author"/></xsl:call-template>
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="source"/></xsl:call-template>
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="id"/></xsl:call-template>
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="location"/></xsl:call-template>
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="date"/></xsl:call-template>
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="notes"/></xsl:call-template>
</xsl:template>

<xsl:template match="assumptions">
{\bf Assumption and Limitations:}
    <xsl:if test="count(assumption) != 0">
\begin{itemize} <xsl:apply-templates select="assumption"/>
\end{itemize}
    </xsl:if>
</xsl:template>

<xsl:template match="assumption">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="text()"/></xsl:call-template>
</xsl:template>

<xsl:template match="requirements">
{\bf Requirements:}
    <xsl:if test="count(requirement) != 0">
\begin{itemize} <xsl:apply-templates select="requirement"/>
\end{itemize}
    </xsl:if>
</xsl:template>

<xsl:template match="requirement">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="text()"/></xsl:call-template>
</xsl:template>

<xsl:template match="library_dependencies">
{\bf Library Dependencies:}
    <xsl:if test="count(library_dependency) != 0">
\begin{itemize} <xsl:apply-templates select="library_dependency"/>
\end{itemize}
    </xsl:if>
</xsl:template>

<xsl:template match="library_dependency">
\item <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="text()"/></xsl:call-template>
</xsl:template>

<xsl:template match="jobs">
    <xsl:apply-templates select="job"/>
</xsl:template>

<xsl:template match="job">
{\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="job_name"/></xsl:call-template>:}

\hspace{0.25in} Return:

\hspace{0.5in}
\begin{tabular}{rl}
Return Type: &amp; {\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="return_type"/></xsl:call-template> } \\
Units: &amp; <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="units"/></xsl:call-template>\\
Description: &amp; <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="comment"/></xsl:call-template>\\
\end{tabular}

\hspace{0.25in} Parameter List:

\begin{table}[hbt]
\begin{center}
\begin{tabular}{|l|l|l|l|l|p{2.5in}|} \hline
{\em Name} &amp; {\em Type} &amp; {\em Dim} &amp; {\em Usage} &amp; {\em Units} &amp; {\em Description} \\ \hline
<xsl:apply-templates select="argument_list"/>
\end{tabular}
\end{center}
\caption{<xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="job_name"/></xsl:call-template>}
\label{tbl:<xsl:value-of select="job_name"/>}
\end{table}
</xsl:template>

<xsl:template match="argument_list">
    <xsl:apply-templates select="arg"/>
</xsl:template>

<xsl:template match="arg">
{\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="arg_name"/></xsl:call-template> } &amp;
{\tt <xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="type"/></xsl:call-template> } &amp;
<xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="dims"/></xsl:call-template> &amp;
<xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="inout"/></xsl:call-template> &amp;
<xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="units"/></xsl:call-template> &amp;
<xsl:call-template name="transform_special"><xsl:with-param name="incoming" select="comment"/></xsl:call-template> \\ \hline
</xsl:template>

<xsl:template name="replace">
  <xsl:param name="string" select="''"/>
  <xsl:param name="pattern" select="''"/>
  <xsl:param name="replacement" select="''"/>
  <xsl:choose>
    <xsl:when test="$pattern != '' and $string != '' and contains($string, $pattern)">
      <xsl:value-of select="substring-before($string, $pattern)"/>
      <xsl:copy-of select="$replacement"/>
      <xsl:call-template name="replace">
        <xsl:with-param name="string" select="substring-after($string, $pattern)"/>
        <xsl:with-param name="pattern" select="$pattern"/>
        <xsl:with-param name="replacement" select="$replacement"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$string"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="transform_special">
  <xsl:param name="incoming" select="''"/>
  <xsl:call-template name="replace">
    <xsl:with-param name="string">
      <xsl:call-template name="replace">
        <xsl:with-param name="string">
          <xsl:call-template name="replace">
            <xsl:with-param name="string">
              <xsl:call-template name="replace">
                <xsl:with-param name="string">
                  <xsl:call-template name="replace">
                    <xsl:with-param name="string">
                      <xsl:call-template name="replace">
                        <xsl:with-param name="string">
                          <xsl:call-template name="replace">
                            <xsl:with-param name="string">
                              <xsl:call-template name="replace">
                                <xsl:with-param name="string">
                                  <xsl:call-template name="replace">
                                    <xsl:with-param name="string">
                                      <xsl:call-template name="replace">
                                        <xsl:with-param name="string">
                                          <xsl:call-template name="replace">
                                            <xsl:with-param name="string">
                                              <xsl:call-template name="replace">
                                                <xsl:with-param name="string" select="normalize-space($incoming)"/>
                                                <xsl:with-param name="pattern" select="'\'"/>
                                                <xsl:with-param name="replacement" select="'$\backslash$'"/>
                                              </xsl:call-template>
                                            </xsl:with-param>
                                            <xsl:with-param name="pattern" select="'&amp;'"/>
                                            <xsl:with-param name="replacement" select="'\&amp;'"/>
                                          </xsl:call-template>
                                        </xsl:with-param>
                                        <xsl:with-param name="pattern" select="'#'"/>
                                        <xsl:with-param name="replacement" select="'\#'"/>
                                      </xsl:call-template>
                                    </xsl:with-param>
                                    <xsl:with-param name="pattern" select="'$'"/>
                                    <xsl:with-param name="replacement" select="'\$'"/>
                                  </xsl:call-template>
                                </xsl:with-param>
                                <xsl:with-param name="pattern" select="'%'"/>
                                <xsl:with-param name="replacement" select="'\%'"/>
                              </xsl:call-template>
                            </xsl:with-param>
                            <xsl:with-param name="pattern" select="'^'"/>
                            <xsl:with-param name="replacement" select="'\^{}'"/>
                          </xsl:call-template>
                        </xsl:with-param>
                        <xsl:with-param name="pattern" select="'{'"/>
                        <xsl:with-param name="replacement" select="'\{'"/>
                      </xsl:call-template>
                    </xsl:with-param>
                    <xsl:with-param name="pattern" select="'}'"/>
                    <xsl:with-param name="replacement" select="'\}'"/>
                  </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="pattern" select="'~'"/>
                <xsl:with-param name="replacement" select="'\~{}'"/>
              </xsl:call-template>
            </xsl:with-param>
            <xsl:with-param name="pattern" select="'_'"/>
            <xsl:with-param name="replacement" select="'\_'"/>
          </xsl:call-template>
        </xsl:with-param>
        <xsl:with-param name="pattern" select="'&gt;'"/>
        <xsl:with-param name="replacement" select="'$&gt;$'"/>
      </xsl:call-template>
    </xsl:with-param>
    <xsl:with-param name="pattern" select="'&lt;'"/>
    <xsl:with-param name="replacement" select="'$&lt;$'"/>
  </xsl:call-template>
</xsl:template>

</xsl:stylesheet>

