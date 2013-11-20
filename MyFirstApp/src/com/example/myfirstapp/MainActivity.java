package com.example.myfirstapp;

import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.concurrent.ExecutionException;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends Activity {
	
	public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";
	private Socket clientSock = null;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {
    	
    	//Create intent for sending message to DisplayMessageActivity
    	Intent intent = new Intent(this, DisplayMessageActivity.class);

    	//Get message from text box
		EditText editText = (EditText) findViewById(R.id.edit_message);
		String str = editText.getText().toString();
		
		//Send message to server and receive transformed message
		String message = null;
		try {
			message = new TCPSendMessage().execute(str).get();
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (ExecutionException e) {
			e.printStackTrace();
		}
		
		//Put message into intent and send to DisplayMessageActivity
		intent.putExtra(EXTRA_MESSAGE, message);
    	startActivity(intent);
    }
    
    private class TCPSendMessage extends AsyncTask<String, Void, String> {
    	
    	protected String doInBackground(String... arg0) {
    		try {
    			//Create a new socket
    			clientSock = new Socket("130.207.114.21", 12003);
    			
    			//Write the message as an output stream
    			PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(clientSock.getOutputStream())), true);
        		out.println(arg0[0]);
        		
        		//Receive the message from the server in unsigned bytes
				DataInputStream input = new DataInputStream(clientSock.getInputStream());
        		String str = new String();
        		for(int i = 0; i < 32; i++) {		
        			str += String.format("%02x", input.readUnsignedByte());
        		}
        		
        		//Close the socket
        		clientSock.close();
        		
        		return str;
    		}
    		catch(UnknownHostException e) {
        		e.printStackTrace();
        	}
        	catch(IOException e) {
        		e.printStackTrace();
        	}
			return null;
    	}
    	
    	protected void onPreExecute() {}
    	protected void onPostExecute(String strOut) {}
    	protected void onProgressUpdate(Void... values) {}
    	
    }
    
}
