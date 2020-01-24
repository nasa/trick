
###############################################################
# External Java libraries
###############################################################

set( JAVA_LIBS
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/bsaf-1.9.2.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jcommon-1.0.23.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jfreechart-1.0.19.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jfreesvg-2.1.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jopt-simple-4.8.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/junit-4.12.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/swingx-1.6.1.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/hamcrest-core-1.3.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/pdfbox-2.0.11.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/fontbox-2.0.11.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/commons-logging-1.2.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-runtime-2.4.0-b180830.0438.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-xjc-2.4.0-b180830.0438.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-jxc-2.4.0-b180830.0438.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-impl-2.4.0-b180830.0438.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-api-2.4.0-b180725.0427.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-core-2.3.0.1.jar
 ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/javax.activation-1.2.0.jar
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/bsaf-1.9.2.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/jdesktop/bsaf/bsaf/1.9.2/bsaf-1.9.2.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jcommon-1.0.23.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/jfree/jcommon/1.0.23/jcommon-1.0.23.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jfreechart-1.0.19.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/jfree/jfreechart/1.0.19/jfreechart-1.0.19.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jfreesvg-2.1.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/jfree/jfreesvg/2.1/jfreesvg-2.1.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jopt-simple-4.8.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/net/sf/jopt-simple/jopt-simple/4.8/jopt-simple-4.8.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/junit-4.12.jar
  COMMAND curl --retry 4 -O -s -S https://repo1.maven.org/maven2/junit/junit/4.12/junit-4.12.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/swingx-1.6.1.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/swinglabs/swingx/1.6.1/swingx-1.6.1.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/hamcrest-core-1.3.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/pdfbox-2.0.11.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/apache/pdfbox/pdfbox/2.0.11/pdfbox-2.0.11.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/fontbox-2.0.11.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/apache/pdfbox/fontbox/2.0.11/fontbox-2.0.11.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/commons-logging-1.2.jar
  COMMAND curl --retry 4 -O -s -S https://repo1.maven.org/maven2/commons-logging/commons-logging/1.2/commons-logging-1.2.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-runtime-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/org/glassfish/jaxb/jaxb-runtime/2.4.0-b180830.0438/jaxb-runtime-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-xjc-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-xjc/2.4.0-b180830.0438/jaxb-xjc-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-jxc-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-jxc/2.4.0-b180830.0438/jaxb-jxc-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-impl-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-impl/2.4.0-b180830.0438/jaxb-impl-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-api-2.4.0-b180725.0427.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-core/2.3.0.1/jaxb-core-2.3.0.1.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/jaxb-core-2.3.0.1.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/javax/xml/bind/jaxb-api/2.4.0-b180725.0427/jaxb-api-2.4.0-b180725.0427.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/libexec/trick/java/lib/javax.activation-1.2.0.jar
  COMMAND curl --retry 4 -O -s -S https://repo.maven.apache.org/maven2/com/sun/activation/javax.activation/1.2.0/javax.activation-1.2.0.jar
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libexec/trick/java/lib
)

###############################################################
# Trick Java libraries
###############################################################

set( JAVA_SRC
  trick/common/ui/components/CommonTreeNode.java
  trick/common/ui/components/DoubleJSlider.java
  trick/common/ui/components/NumberTextField.java
  trick/common/ui/components/FontChooser.java
  trick/common/ui/panels/AnimationPlayer.java
  trick/common/ui/panels/ConnectionStatusBar.java
  trick/common/ui/panels/DataPanel.java
  trick/common/ui/panels/DynamicTree.java
  trick/common/ui/panels/FindBar.java
  trick/common/ui/panels/ListPanel.java
  trick/common/ui/panels/SmallTrickIconLabel.java
  trick/common/ui/TrickFileFilter.java
  trick/common/ui/UIUtils.java
  trick/common/utils/vs/VSBoolean.java
  trick/common/utils/vs/VSByte.java
  trick/common/utils/vs/VSDouble.java
  trick/common/utils/vs/VSFloat.java
  trick/common/utils/vs/VSInteger.java
  trick/common/utils/vs/VSLong.java
  trick/common/utils/vs/VSShort.java
  trick/common/utils/vs/VSString.java
  trick/common/utils/vs/VSValue.java
  trick/common/utils/vs/Variable.java
  trick/common/utils/vs/VariableListener.java
  trick/common/utils/vs/VariableServerFluent.java
  trick/common/utils/vs/VariableTracker.java
  trick/common/utils/BinaryDataReader.java
  trick/common/utils/CSVDataReader.java
  trick/common/utils/ErrorChecker.java
  trick/common/utils/LogHeaderReader.java
  trick/common/utils/LogVar.java
  trick/common/utils/SortedListModel.java
  trick/common/utils/TrickColors.java
  trick/common/utils/UnitInfixExpression.java
  trick/common/utils/UnitType.java
  trick/common/utils/VariableServerConnection.java
  trick/common/utils/XMLCreator.java
  trick/common/utils/DataReader.java
  trick/common/RunTimeTrickApplication.java
  trick/common/TrickApplication.java
  trick/dataproducts/plot/utils/editor/DefaultLogAxisEditor.java
  trick/dataproducts/plot/utils/editor/TrickAxisEditor.java
  trick/dataproducts/plot/utils/editor/TrickChartEditor.java
  trick/dataproducts/plot/utils/editor/TrickChartEditorFactory.java
  trick/dataproducts/plot/utils/editor/TrickChartEditorManager.java
  trick/dataproducts/plot/utils/editor/TrickNumberAxisEditor.java
  trick/dataproducts/plot/utils/editor/TrickPlotEditor.java
  trick/dataproducts/plot/utils/editor/TrickPolarPlotEditor.java
  trick/dataproducts/plot/utils/editor/TrickTitleEditor.java
  trick/dataproducts/plot/utils/editor/TrickValueAxisEditor.java
  trick/dataproducts/plot/utils/PlotUtils.java
  trick/dataproducts/plot/utils/TrickChart.java
  trick/dataproducts/plot/utils/TrickChartControlPanel.java
  trick/dataproducts/plot/utils/TrickChartPanel.java
  trick/dataproducts/plot/utils/TrickChartTheme.java
  trick/dataproducts/plot/utils/TrickFrame.java
  trick/dataproducts/plot/utils/TrickTableFrame.java
  trick/dataproducts/plot/utils/TrickXYLineAndShapeRenderer.java
  trick/dataproducts/plot/utils/TrickXYSeries.java
  trick/dataproducts/plot/utils/TrickChartFrame.java
  trick/dataproducts/plot/utils/TrickXYPlot.java
  trick/dataproducts/plot/JXPlotApplication.java
  trick/dataproducts/trickdp/utils/DPRemoteCallInterface.java
  trick/dataproducts/trickdp/utils/DPRemoteCallInterfaceImpl.java
  trick/dataproducts/trickdp/utils/PDFBooklet.java
  trick/dataproducts/trickdp/utils/TrickDPActionController.java
  trick/dataproducts/trickdp/TrickDPApplication.java
  trick/dataproducts/trickqp/utils/DataTransferHandler.java
  trick/dataproducts/trickqp/utils/Product.java
  trick/dataproducts/trickqp/utils/ProductAxis.java
  trick/dataproducts/trickqp/utils/ProductCurve.java
  trick/dataproducts/trickqp/utils/ProductDataPanel.java
  trick/dataproducts/trickqp/utils/ProductDomParser.java
  trick/dataproducts/trickqp/utils/ProductExternalFunction.java
  trick/dataproducts/trickqp/utils/ProductTable.java
  trick/dataproducts/trickqp/utils/ProductTree.java
  trick/dataproducts/trickqp/utils/ProductVarcase.java
  trick/dataproducts/trickqp/utils/QPRemoteCallInterface.java
  trick/dataproducts/trickqp/utils/QPRemoteCallInterfaceImpl.java
  trick/dataproducts/trickqp/utils/VarListPanel.java
  trick/dataproducts/trickqp/utils/CommonProduct.java
  trick/dataproducts/trickqp/utils/ProductPage.java
  trick/dataproducts/trickqp/utils/ProductPlot.java
  trick/dataproducts/trickqp/utils/ProductColumn.java
  trick/dataproducts/trickqp/utils/ProductMeasurement.java
  trick/dataproducts/trickqp/utils/ProductVar.java
  trick/dataproducts/trickqp/utils/ProductXMLCreator.java
  trick/dataproducts/trickqp/utils/TrickQPActionController.java
  trick/dataproducts/trickqp/TrickQPApplication.java
  trick/dataproducts/utils/FileTreeNode.java
  trick/dataproducts/utils/FileTreePanel.java
  trick/dataproducts/utils/Session.java
  trick/dataproducts/utils/SessionDomParser.java
  trick/dataproducts/utils/SessionRun.java
  trick/dataproducts/utils/SessionRunTransferHandler.java
  trick/dataproducts/utils/SessionXMLCreator.java
  trick/dataproducts/utils/SimDPTree.java
  trick/dataproducts/utils/SimRunDPTree.java
  trick/dataproducts/utils/SimRunTree.java
  trick/dataproducts/DataProductsApplication.java
  trick/dre/DreApplication.java
  trick/montemonitor/MonteMonitorApplication.java
  trick/montemonitor/Slave.java
  trick/mtv/MtvApp.java
  trick/mtv/MtvView.java
  trick/sie/utils/SearchListener.java
  trick/sie/utils/SearchPanel.java
  trick/sie/utils/Searcher.java
  trick/sie/utils/SieResourceDomParser.java
  trick/sie/utils/SieTemplate.java
  trick/sie/utils/SieTree.java
  trick/sie/utils/SieTreeModel.java
  trick/sie/utils/SieVariableTree.java
  trick/sie/utils/TreeModelExclusionFilter.java
  trick/sie/utils/TreeModelFilter.java
  trick/sie/utils/TreeModelSortingFilter.java
  trick/sie/utils/VariableList.java
  trick/sie/utils/SieEnumeration.java
  trick/sie/SieApplication.java
  trick/simcontrol/utils/SimControlActionController.java
  trick/simcontrol/utils/SimState.java
  trick/simcontrol/SimControlApplication.java
  trick/sniffer/SimSnifferApplication.java
  trick/sniffer/SimulationInformation.java
  trick/sniffer/SimulationListener.java
  trick/sniffer/SimulationSniffer.java
  trick/test/Client.java
  trick/tv/StripChart.java
  trick/tv/TVApplication.java
  trick/tv/TVBean.java
  trick/tv/TVBoolean.java
  trick/tv/TVDouble.java
  trick/tv/TVEnumeration.java
  trick/tv/TVFloat.java
  trick/tv/TVString.java
  trick/tv/TVVariableTree.java
  trick/tv/TrickViewFluent.java
  trick/tv/VariableTable.java
  trick/tv/DoubleComboBox.java
  trick/tv/StripChartManager.java
  trick/tv/TVByte.java
  trick/tv/TVLong.java
  trick/tv/TVShort.java
  trick/tv/TVInteger.java
  trick/vc/VariableCounter.java
  trick/Template.java
)

set( JAVA_RESOURCES
  trick/common/resources/RunTimeTrickApplication.properties
  trick/common/resources/TrickApplication.properties
  trick/common/resources/filenew.gif
  trick/common/resources/fileopen.gif
  trick/common/resources/filesave.gif
  trick/common/resources/firefox_folder_closed.gif
  trick/common/resources/firefox_folder_open.gif
  trick/common/resources/gnome-fs-regular.gif
  trick/common/resources/page2.gif
  trick/common/resources/plot.gif
  trick/common/resources/program.gif
  trick/common/resources/program_in.gif
  trick/common/resources/program_out.gif
  trick/common/resources/table_small.gif
  trick/common/resources/toplevel.gif
  trick/common/resources/trick.gif
  trick/common/resources/trick_icon.png
  trick/common/resources/trick_small.gif
  trick/common/resources/variable.gif
  trick/common/resources/x_variable.gif
  trick/common/resources/y_variable.gif
  trick/dataproducts/plot/resources/JXPlotApplication.properties
  trick/dataproducts/resources/DataProductsApplication.properties
  trick/dataproducts/resources/gnuplot_off.gif
  trick/dataproducts/resources/gnuplot_on.gif
  trick/dataproducts/resources/plot_contrast.gif
  trick/dataproducts/resources/plot_coplot.gif
  trick/dataproducts/resources/plot_error.gif
  trick/dataproducts/resources/plot_single.gif
  trick/dataproducts/resources/table_callback1.gif
  trick/dataproducts/resources/table_error_callback.gif
  trick/dataproducts/trickdp/resources/TrickDPApplication.properties
  trick/dataproducts/trickdp/resources/adobe_pdf.gif
  trick/dataproducts/trickdp/resources/help/Help.hs
  trick/dataproducts/trickdp/resources/help/HelpTOC.xml
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/DOCS
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/DOCS.TAB
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/OFFSETS
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/POSITIONS
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/SCHEMA
  trick/dataproducts/trickdp/resources/help/JavaHelpSearch/TMAP
  trick/dataproducts/trickdp/resources/help/Map.jhm
  trick/dataproducts/trickdp/resources/help/html/faq.html
  trick/dataproducts/trickdp/resources/help/html/generalusage.html
  trick/dataproducts/trickdp/resources/help/html/howtoorganize.html
  trick/dataproducts/trickdp/resources/help/html/importingdata.html
  trick/dataproducts/trickdp/resources/help/html/intro.html
  trick/dataproducts/trickdp/resources/help/html/quickplot.html
  trick/dataproducts/trickdp/resources/help/html/usinggnuplot.html
  trick/dataproducts/trickdp/resources/ps_coplot.gif
  trick/dataproducts/trickdp/resources/ps_error.gif
  trick/dataproducts/trickdp/resources/ps_single.gif
  trick/dataproducts/trickdp/resources/quickplot.gif
  trick/dataproducts/trickqp/resources/TrickQPApplication.properties
  trick/dataproducts/trickqp/resources/function.gif
  trick/dataproducts/trickqp/resources/help/Help.hs
  trick/dataproducts/trickqp/resources/help/HelpTOC.xml
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/DOCS
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/DOCS.TAB
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/OFFSETS
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/POSITIONS
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/SCHEMA
  trick/dataproducts/trickqp/resources/help/JavaHelpSearch/TMAP
  trick/dataproducts/trickqp/resources/help/Map.jhm
  trick/dataproducts/trickqp/resources/help/html/anatomyofaplot.html
  trick/dataproducts/trickqp/resources/help/html/general.html
  trick/dataproducts/trickqp/resources/help/html/guilayout.html
  trick/dataproducts/trickqp/resources/help/html/intro.html
  trick/dataproducts/trickqp/resources/help/html/leftovers.html
  trick/dataproducts/trickqp/resources/help/html/minitutorial.html
  trick/dre/resources/DreApplication.properties
  trick/montemonitor/resources/MonteMonitorApplication.properties
  trick/montemonitor/resources/dice.gif
  trick/montemonitor/resources/disconnected.png
  trick/montemonitor/resources/finished.png
  trick/montemonitor/resources/initializing.png
  trick/montemonitor/resources/killed.png
  trick/montemonitor/resources/ready.png
  trick/montemonitor/resources/running.png
  trick/montemonitor/resources/stopped.png
  trick/montemonitor/resources/stopping.png
  trick/montemonitor/resources/uninitialized.png
  trick/montemonitor/resources/unknown.png
  trick/montemonitor/resources/unresponsive-running.png
  trick/montemonitor/resources/unresponsive-stopping.png
  trick/montemonitor/resources/unresponsive.png
  trick/mtv/resources/MtvAboutBox.properties
  trick/mtv/resources/MtvApp.properties
  trick/mtv/resources/MtvView.properties
  trick/mtv/resources/busyicons/busy-icon0.png
  trick/mtv/resources/busyicons/busy-icon1.png
  trick/mtv/resources/busyicons/busy-icon10.png
  trick/mtv/resources/busyicons/busy-icon11.png
  trick/mtv/resources/busyicons/busy-icon12.png
  trick/mtv/resources/busyicons/busy-icon13.png
  trick/mtv/resources/busyicons/busy-icon14.png
  trick/mtv/resources/busyicons/busy-icon2.png
  trick/mtv/resources/busyicons/busy-icon3.png
  trick/mtv/resources/busyicons/busy-icon4.png
  trick/mtv/resources/busyicons/busy-icon5.png
  trick/mtv/resources/busyicons/busy-icon6.png
  trick/mtv/resources/busyicons/busy-icon7.png
  trick/mtv/resources/busyicons/busy-icon8.png
  trick/mtv/resources/busyicons/busy-icon9.png
  trick/mtv/resources/busyicons/idle-icon.png
  trick/mtv/resources/delete_22x22.gif
  trick/mtv/resources/fileopen.gif
  trick/mtv/resources/filesave.gif
  trick/mtv/resources/trick4.gif
  trick/mtv/resources/trick_icon.png
  trick/sie/resources/SieApplication.properties
  trick/simcontrol/resources/SimControlApplication.properties
  trick/simcontrol/resources/mtv_22x22.png
  trick/simcontrol/resources/throttle_22x22.png
  trick/simcontrol/resources/tv_22x22.png
  trick/sniffer/resources/SimSnifferApplication.properties
  trick/sniffer/resources/nose.gif
  trick/tv/jaxb.index
  trick/tv/resources/CP.gif
  trick/tv/resources/TVApplication.properties
  trick/tv/resources/delete_22x22.gif
  trick/tv/resources/fileimport.gif
  trick/tv/resources/kmplot.gif
  trick/tv/resources/trickView.xsd
  trick/tv/resources/tv.xsd
  trick/tv/resources/tv_32x32.gif
  trick/tv/resources/tv_off.png
  trick/tv/resources/tv_on.png
)

add_jar( trick_jar
  SOURCES ${JAVA_SRC} ${JAVA_RESOURCES}
  INCLUDE_JARS ${JAVA_LIBS}
  OUTPUT_NAME trick
  OUTPUT_DIR ${CMAKE_BINARY_DIR}/libexec/trick/java/dist
)

file(COPY trick/common/resources/trick_icon.png DESTINATION ${CMAKE_BINARY_DIR}/libexec/trick/java/resources)
