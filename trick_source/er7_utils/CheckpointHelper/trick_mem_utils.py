def create_new_sim_object( simObjName, typeName, ctrString = "" ):
  global externAllocs
  TRICK_DEFAULT_PHASE = 60000
  externAllocs.simObjectAllocs.append((simObjName, typeName, ctrString))

  if not ctrString:
    ctrString = "trick." + typeName + "()"
  else:
    ctrString = "trick." + typeName + ctrString
     
  cmdStr = simObjName + " = " + ctrString + "\n"
  cmdStr += "globals()[\'" + simObjName + "\'] = " + simObjName + "\n"

  if "DRAscii" in typeName or "DRBinary" in typeName or "DRHDF5" in typeName:
     cmdStr += simObjName + ".declare_memory()\n"
  else:
     cmdStr += "trick.exec_add_sim_object(" + simObjName + ", \'" + simObjName + "\' )\n"
     cmdStr += "trick.TMM_declare_ext_var(" + simObjName + ", trick.TRICK_STRUCTURED, \'" + typeName + "\', 0, \'" + simObjName + "\', 0, None)\n"

  cmdStr += "newSO = " + simObjName + "\n"
  exec(cmdStr)
  return newSO

def create_new_instance( objectName, typeName, ctrString = "", mmName = ""):
  global externAllocs
  externAllocs.objectAllocs.append((objectName, typeName, ctrString, mmName))

  if not ctrString:
    ctrString = "trick." + typeName + "()"
  else:
    ctrString = "trick." + typeName + ctrString

  cmdStr = objectName + " = " + ctrString + "\n"
  cmdStr += "globals()[\'" + objectName + "\'] = " + objectName + "\n"
  if not mmName:
     cmdStr += "trick.TMM_declare_ext_var(" + objectName + ", trick.TRICK_STRUCTURED, \'" + typeName + "\', 0, \'" + objectName + "\', 0, None)\n"
  else:
     cmdStr += "trick.TMM_declare_ext_var(" + objectName + ", trick.TRICK_STRUCTURED, \'" + mmName + "\', 0, \'" + objectName + "\', 0, None)\n"
  cmdStr += "newObj = " + objectName + "\n"
  exec(cmdStr)
  return newObj

class ExternAlloc():
  def __init__(self):
     self.simObjectAllocs = []
     self.objectAllocs = []

  def printEntries(self):
     for alloc in self.simObjectAllocs:
        print "SimObject", alloc[0], "of type", alloc[1], "allocated in input file"
     for alloc in self.objectAllocs:
        print "Object", alloc[0], "of type", alloc[1], "allocated in input file"
     return
  
  def writeChkptFile(self, chkpt_file):
     path = chkpt_file
     file = open(path, 'w')
     file.write("oldAllocs = ExternAlloc()\n")
     file.write("oldAllocs.simObjectAllocs = list(externAllocs.simObjectAllocs)\n")
     file.write("oldAllocs.objectAllocs = list(externAllocs.objectAllocs)\n")
     file.write("newAllocs = ExternAlloc()\n")
     for alloc in self.simObjectAllocs:
        file.write("newAllocs.simObjectAllocs.append((\"" + alloc[0] + "\",\"" + alloc[1] + "\",\"" + alloc[2] +"\"))\n")
     for alloc in self.objectAllocs:
        file.write("newAllocs.objectAllocs.append((\"" + alloc[0] + "\",\"" + alloc[1] + "\",\"" + alloc[2] +"\", \"" + alloc[3] + "\"))\n")
     file.write("for alloc in newAllocs.simObjectAllocs:\n")
     file.write("  if alloc not in oldAllocs.simObjectAllocs:\n")
     file.write("     create_new_sim_object( alloc[0], alloc[1], alloc[2])\n")
     file.write("     print \"Allocated \" + alloc[0]\n")
     file.write("  else:\n")
     file.write("     oldAllocs.simObjectAllocs.remove( alloc )\n")
     file.write("for alloc in newAllocs.objectAllocs:\n")
     file.write("  if alloc not in oldAllocs.objectAllocs:\n")
     file.write("     create_new_instance( alloc[0], alloc[1], alloc[2], alloc[3])\n")
     file.write("     print \"Allocated \" + alloc[0]\n")
     file.write("  else:\n")
     file.write("     oldAllocs.objectAllocs.remove( alloc )\n")
     file.write("for alloc in oldAllocs.simObjectAllocs:\n")
     file.write("     exec(\"ptr = \" + alloc[0])\n")
     file.write("     trick.exec_remove_sim_object( ptr ) \n")
     file.write("     trick.TMM_delete_extern_var_a( ptr )\n")
     file.write("     del globals()[alloc[0]]\n")
     file.write("     print \"Deleting \" + alloc[0]\n")
     file.write("     externAllocs.simObjectAllocs.remove( alloc )\n")
     file.write("for alloc in oldAllocs.objectAllocs:\n")
     file.write("     trick.TMM_delete_extern_var_n( alloc[0] )\n")
     file.write("     del globals()[alloc[0]]\n")
     file.write("     print \"Deleting \" + alloc[0]\n")
     file.write("     externAllocs.objectAllocs.remove( alloc )\n")
     return

if "externAllocs" not in globals():
  global externAllocs
  externAllocs = ExternAlloc()
