#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tkRAD

# we don't need a RADApplication
# for such a simple app,
# let's get a Main Window instead.

class EthercatMasterGui (tkRAD.RADXMLMainWindow):

    def init_widget (self, **kw):

        # set a window caption with i18n support
        self.title(_("EtherCAT Master GUI"))

        # set the GUI with XML
        xml = """
            <tkwidget>
                <ttklabelframe
                    text="EtherCAT master information"
                    padding="5px"
                    layout="pack"
                    layout_options="padx=5, pady=5"
                    resizable="yes"
                >
                    <text
                        name="ec_master_out"
                        resizable="yes"
                        layout="pack"
                    />
                    <button
                        text="ethercat master"
                        command="@EthercatMaster"
                        layout="pack"
                    />
                </ttklabelframe>
                <ttkbutton
                    text="Quit this app"
                    command="@quit"
                    layout="pack"
                />
            </tkwidget>
        """

        # build the GUI
        self.xml_build(xml)

        # connect XML events
        self.events.connect("EthercatMaster", self.call_ethercat_master)

    # end def

    def call_ethercat_master(self, *args, **kw):
        import subprocess
        import tkinter
        s = subprocess.check_output("""ethercat master""", shell=True, universal_newlines=True)
        textbox = self.mainframe.ec_master_out
        if textbox:
            textbox.delete('1.0', tkinter.END)
            textbox.insert(tkinter.END, s)
            textbox.insert(tkinter.END, '\n')
        else:
            print("Couldn't access textbox ec_master_out")

# end class SimpleCalculator

# run main window as application
EthercatMasterGui().run()
