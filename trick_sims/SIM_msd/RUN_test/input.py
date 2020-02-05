dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)
trick.var_debug(3)
exec(open("Modified_data/realtime.py").read())

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
trick.set_var_server_info_msg_on()

MsdGui_path = "models/graphics/dist/MsdGui.jar"

if (os.path.isfile(MsdGui_path)) :
    MsdGui_cmd = "java -jar " \
                  + MsdGui_path \
                  + " " + str(varServerPort) + " &" ;
    print(MsdGui_cmd)
    os.system( MsdGui_cmd);
else :
    print('==================================================================================')
    print('MsdGui needs to be built. Please \"cd\" into models/Graphics and type \"make\".')
    print('==================================================================================')

trick.stop(300.0)
