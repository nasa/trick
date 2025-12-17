
import wx


class MyFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(300,200))

        self.CreateStatusBar()

        filemenu = wx.Menu()
        menuAbout = filemenu.Append(wx.ID_ABOUT, "&About"," Information about this program")
        filemenu.AppendSeparator()
        menuExit = filemenu.Append(wx.ID_EXIT,"E&xit"," Therminate the program")

        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File")
        self.SetMenuBar(menuBar)

        # Set events.
        self.Bind(wx.EVT_MENU, self.OnAbout, menuAbout)
        self.Bind(wx.EVT_MENU, self.OnExit, menuExit)

        # Add a panel and some controls to display the size and position
        panel = wx.Panel(self, -1)
        label1 = wx.StaticText(panel, -1, "X_pos:")
        label2 = wx.StaticText(panel, -1, "Y_pos:")
        self.xpos = wx.TextCtrl(panel, -1, "")
        self.ypos = wx.TextCtrl(panel, -1, "")
        self.panel = panel

        self.xpos.sim_var = 'ball.obj.state.input.position[0]'
        self.ypos.sim_var = 'ball.obj.state.input.position[1]'

        self.xpos.ChangeValue("%s" % eval(self.xpos.sim_var))
        self.ypos.ChangeValue("%s" % eval(self.ypos.sim_var))

        self.Bind(wx.EVT_TEXT, self.SetFloatValue, self.xpos)
        self.Bind(wx.EVT_TEXT, self.SetFloatValue, self.ypos)

        # Use some sizers for layout of the widgets
        sizer = wx.FlexGridSizer(1, 4, 5, 5)
        sizer.Add(label1, flag=wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(self.xpos)
        sizer.Add(label2, flag=wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(self.ypos)

        border = wx.BoxSizer()
        border.Add(sizer, 0, wx.ALL, 15)

        self.start_button = wx.Button(panel, -1, "Start Sim")
        sizer.Add(self.start_button, flag=wx.ALIGN_RIGHT)
        self.Bind(wx.EVT_BUTTON, self.OnExit)

        panel.SetSizerAndFit(border)

        self.Fit()
        self.Show(True)

    def OnAbout(self,e):
        # A message dialog box with an OK button. wx.OK is a standard ID in wxWidgets.
        dlg = wx.MessageDialog( self, "A small text editor", "About Sample Editor", wx.OK)
        dlg.ShowModal() # Show it
        dlg.Destroy() # finally destroy it when finished.

    def OnExit(self,e):
        self.Close(True)  # Close the frame.

    def SetFloatValue(self,e):
        # get the changed value of the text field
        value =  e.GetEventObject().GetValue()
        # test to see if the value is a float or not
        try:
            test = float(value)
        except ValueError: 
            pass
        else:
            # create a statement that assigns the underlying simulation variable to the value of the text field
            statement = e.GetEventObject().sim_var + " = " + str(test)
            # assign the value
            exec(statement)

app = wx.App(False)
frame = MyFrame(None, 'Ball starting conditions editor')
app.MainLoop()

trick.exec_set_terminate_time(300.0)

