dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)
execfile("Modified_data/realtime.py")

#==========================================
# Start the display VarServer Client
#==========================================
varServerPort = trick.var_server_get_port();
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
