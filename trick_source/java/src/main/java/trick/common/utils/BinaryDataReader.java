
//========================================
//    Package
//========================================
package trick.common.utils;

//========================================
//    Imports
//========================================
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import trick.common.utils.LogVar.DisplayType;
import trick.common.utils.LogVar.TrickType;
import trick.dataproducts.trickqp.utils.ProductVar;


/**
 * This data reader is for retrieving the recorded variable list out of Trick recorded binary data.
 * 
 *
 * @since Trick 10
 */
public class BinaryDataReader extends DataReader {
	
	 
    //========================================
    //    Public data
    //========================================
	

    //========================================
    //    Protected data
    //========================================
	


    //========================================
    //    Private Data
    //========================================
    private int numParams;
    private long headerBytes;
    private boolean isLittleEndian;

    private FileInputStream fileInputStream;

    //========================================
    //    Constructors
    //========================================
    /**
     * Constructs a reader with specified file name.
     *
     * @param fileName    The name of the file.
     */
    public BinaryDataReader(String fileName) {
        super(fileName);
    }

    /**
     * Constructs a reader with specified file.
     *
     *  @param file        An instance of {@link File} for the reader to read.
     */
    public BinaryDataReader(File file) {
        super(file);
    }

    
    //========================================
    //    Set/Get methods
    //========================================



    //========================================
    //    Methods
    //========================================      
    /**
     * Processes the recorded data. Required by {@link DataReader}.
     */
    @Override
	protected void processHeader() throws FileNotFoundException, IOException {       
        FileInputStream inFile = new FileInputStream(dataFile);
        
        // 10 bytes are for endian spec      
        String endianStr = readBytesToString(inFile, 10);
        headerBytes += 10;
                
        if (endianStr.endsWith("-L")) {
            isLittleEndian = true;
        }
        
        numParams = readBytesToInt(inFile, 4);
        headerBytes += 4;
        
        // now go through the header for this data file
        for (int k = 0; k < numParams; k++) {
            // 4 bytes for the length of this param name
            int paramNameLen = readBytesToInt(inFile, 4);
            headerBytes += 4;
            
            // paramNameLen bytes for the param name
            String paramName = readBytesToString(inFile, paramNameLen);
            headerBytes += paramNameLen;
            
            LogVar varObj = new LogVar(paramName);
            
            // 4 bytes for the length of the units
            int unitsLen = readBytesToInt(inFile, 4);
            headerBytes += 4;
            
            // unitsLen bytes for the units
            // TODO: unit conversion for Trick-05*
            varObj.setUnits(readBytesToString(inFile, unitsLen));
            headerBytes += unitsLen;
            
            // 4 bytes for the type id
            int typeId = readBytesToInt(inFile, 4);
            headerBytes += 4;
            
            int size = readBytesToInt(inFile, 4);
            headerBytes += 4;
            
            TrickType varType = TrickType.TRICK_DOUBLE;
            //varObj.setType(getType(typeId));
            if (typeId > -1 && typeId < TrickType.values().length) {
            	varType = TrickType.values()[typeId];
            }
            varObj.setType(varType);
            varObj.setSize(size);
            
            varObj.setDisplay(DisplayType.NORMAL);
            varObj.setLevel(0);
            varObj.setCount(1);
            varObj.setIsFromLog(true);
            varObj.setRunDir(dataFile.getParent());
            recordedVarList.add(varObj);
        }
        
        if (inFile != null) {
        	inFile.close();
        }
    }
        
    /**
     * Helper method for reading specified number of bytes from a binary file and convert it to an int.
     */
    private int readBytesToInt(FileInputStream inFile, int numBytes) throws IOException {    	
    	ByteBuffer byteBuf = readBytes(inFile, numBytes);
    	
    	return byteBuf.getInt();
    }
   
    /**
     * Helper method for reading specified number of bytes from a binary file with correct format.    
     */
    private ByteBuffer readBytes(FileInputStream inFile, int numBytes) throws IOException {
        byte[] bytes = new byte[numBytes];
    	inFile.read(bytes);
    	ByteBuffer byteBuf = ByteBuffer.wrap(bytes);
    	// Java native format is big endian, no need to set the order for bit endian.
    	if (isLittleEndian) {
    		byteBuf.order(ByteOrder.LITTLE_ENDIAN);
    	}
        return byteBuf;
    }
    
    /**
     * Helper method for reading specified number bytes from a binary file and convert it to a string.
     */
    private String readBytesToString(FileInputStream inFile, int numBytes) throws IOException {
    	byte[] bytes = new byte[numBytes];
    	inFile.read(bytes);   	
    	return new String(bytes);
    }
       
	@Override
	public void beginRead(ProductVar var) throws IOException {
		super.beginRead(var);
		fileInputStream = new FileInputStream(dataFile);
		fileInputStream.skip(headerBytes);
	}
	
	@Override
	public double getVarValue() throws IOException {
		double varData = Double.NaN;
        boolean isDone = false;
        
        while (true) {
            boolean varFound = false;
            boolean timeFound = false;
            boolean goodTime = true;
            
            for (int j = 0; j < numParams; j++) {
            	LogVar thisVar = (LogVar)recordedVarList.get(j);
                byte[] bytes = new byte[thisVar.getSize()];
                if (fileInputStream.read(bytes) == -1) {
                	isDone = true;
                    break;
                }
                
                ByteBuffer byteBuf = ByteBuffer.wrap(bytes);
                // Java native format is big endian, no need to set the order for bit endian.
                if (isLittleEndian) {
                    byteBuf.order(ByteOrder.LITTLE_ENDIAN);
                }
                
                if (j == timeIndex) {
                    timeFound = true;
                    theTimeValue = byteBuf.getDouble();                   
                    goodTime = isTimeInRange(theTimeValue);
                }
                
                if (j == theVarIndex) {
                    if (timeIndex == theVarIndex) {
                        varData = theTimeValue;
                    } else {
                    	if (thisVar.getType() != null) {
	                    	switch (thisVar.getType()) {                 	
	                    		//case TRICK_VOID:
	                    		//case TRICK_CHARACTER:
	                    		//case TRICK_UNSIGNED_CHARACTER:	
	                    		//case TRICK_STRING:
	                    		case TRICK_SHORT:
	                    		case TRICK_UNSIGNED_SHORT:
	                    			varData = byteBuf.getShort();
	                    			break;
	                    		case TRICK_INTEGER:
	                    		case TRICK_UNSIGNED_INTEGER:
	                    			varData = byteBuf.getInt();
	                    			break;
	                    		case TRICK_LONG:
	                    		case TRICK_UNSIGNED_LONG:
	                    		case TRICK_LONG_LONG:
	                    		case TRICK_UNSIGNED_LONG_LONG:
	                    			varData = byteBuf.getLong();
	                    			break;
	                    		case TRICK_FLOAT:
	                    			varData = byteBuf.getFloat();
	                    			break;
	                    		case TRICK_DOUBLE:
	                    			varData = byteBuf.getDouble();
	                    			break;
	                    		case TRICK_BITFIELD:
	                    		case TRICK_UNSIGNED_BITFIELD:
	                    			if (thisVar.getSize() == 1) {
	                    				varData = byteBuf.getChar();
	                    			} else if (thisVar.getSize() == 2) {
	                    				varData = byteBuf.getShort();
	                    			} else if (thisVar.getSize() == 4) {
	                    				varData = byteBuf.getInt();
	                    			}
	                    			break;
	                    		default:
	                    			break;
	                    		//case TRICK_FILE_PTR:
	                    		//case TRICK_BOOLEAN:
	                    		//case TRICK_WCHAR:
	                    		//case TRICK_WSTRING:
	                    		//case TRICK_VOID_PTR:
	                    		//case TRICK_ENUMERATED:
	                    		//case TRICK_STRUCTURED:
	                    		//case TRICK_USER_DEFINED_TYPE:
	                    		//case TRICK_NUMBER_OF_TYPES:
	                    	}
                    	}
                    }
                    varFound = true;
                }
                
                if (varFound && timeFound) {
                    if (goodTime) {
                    	try {
                        	varData = applyVarProperties(varData);
                        	isDone = true;
                        } catch (UnitType.IllegalUnitConversionException e) {       		
                    		System.out.println(e.getMessage());
                    		System.out.println(e.getCause());
                    		// set varFound false as no need to go through all log data
                    		// if unit conversion can't be performed for the variable
                    		varFound = false;
                    		break;
                        }
                    }
                    int needToSkipNum = Math.max(timeIndex, theVarIndex);
                    for (int k = needToSkipNum + 1; k < recordedVarList.size(); k ++) {
                    	LogVar theVar = (LogVar)recordedVarList.get(k);
                    	fileInputStream.skip(theVar.getSize());
                    }                   
                    break;
                } 
            }
            if (!varFound || isDone) {
                break;
            }
        }        
        return varData;
	}
	
	@Override
	public boolean isEnd() {
		boolean ret = false;
		if (fileInputStream == null) {
			ret = true;
		} 
		try {
			if (fileInputStream.available() < 1) {
				ret = true;
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return ret;
	}

	@Override
	public void endRead() {
		if (fileInputStream != null) {
            try {
				fileInputStream.close();
			} catch (IOException e) {				
				e.printStackTrace();
			}
        }
	}
       
}
