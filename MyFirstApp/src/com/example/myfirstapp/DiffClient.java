package com.example.myfirstapp;

<<<<<<< HEAD
import java.io.File;

public class DiffClient {

	Message rcvMessage;
	
	public DiffClient(Message rcvMessage) {
		rcvMessage = this.rcvMessage;
	}

	public static void main(String[] args) {
		
	}
	
	public String fileCompare(Message rcvMessage) {
		
	}
	
	public String listFilesForFolder(final File folder) {
		String fileStr = new String();
        for (final File fileEntry : folder.listFiles()) {
            if (fileEntry.isDirectory()) {
                listFilesForFolder(fileEntry);
            } else {
                fileStr += fileEntry.getName() + "\n";
            }
        }
        return fileStr;
    }
=======
public class DiffClient {

	
	/*public static DiffClient(Message rcvMessage, Message sndMessage) {
		
	}
	
	public void getFiles(File f){
        File files[];
        if(f.isFile())
            Log.d("debugging", f.getAbsolutePath());
        else{
            files = f.listFiles();
            for (int i = 0; i < files.length; i++) {
                getFiles(files[i]);
            }
        }
    }*/
	
	public static void main(String[] args) {

	}
>>>>>>> d90bcd1515b010144950eb4f8881a74dc710f647

}
