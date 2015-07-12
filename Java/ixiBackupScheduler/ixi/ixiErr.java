package ixi;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import javax.swing.JOptionPane;


public class ixiErr
{

	public class ixiErrInfo
	{
		public int ixiErrCode;
		public String ixiErrReport;
		public String ixiEngineState;
		
		ixiErrInfo( int c, String er, String es )
		{
			ixiEngineState = es;
			ixiErrReport = er;
			ixiErrCode = c;
		}
	}

	public Calendar ErrCal;
	public int WriteErrorToFile;
	public int WriteErrorToPopup;
	public int WriteErrorToConsole;
	public ixiErrInfo ErrorInfoList[];
	public static int TotalErrorCount;
	
	public String ErrorFile;

	private SimpleDateFormat errDateFormatter = new SimpleDateFormat("MM-dd-HH-mm-ss");
	
	
	public ixiErr()
	{
		TotalErrorCount = 0;
		ErrorInfoList = new ixiErrInfo[1024];		
	}
	
	private String getTimeStamp()
	{
		ErrCal = Calendar.getInstance();
		ErrCal.setTime( new Date() );
		return errDateFormatter.format(ErrCal.getTime());
	}
	
	public void ReportError( int ErrCode, String ErrReport, String EngineState )
	{
		// Create the error report
		ErrorInfoList[ TotalErrorCount++ ] = new ixiErrInfo( ErrCode, ErrReport, EngineState );
		
		// Display Setting
		if ( WriteErrorToFile == 1 )
		{
			WriteErrorsToFile();
		}
		if ( WriteErrorToPopup == 1 || ErrCode ==9 )
		{
			JOptionPane.showMessageDialog( null, ErrReport ,"Error",JOptionPane.OK_CANCEL_OPTION); 
		}
		if ( WriteErrorToConsole == 1 )
		{
			System.out.println( " Error Number : [" + TotalErrorCount + "]" );
			System.out.println( " Error Code   : [" + ErrCode + "]" );
			System.out.println( " Engine State : [" + EngineState + "]" );
			System.out.println( " Report       :\n\n " + ErrReport );
			System.out.println( "\n [" + getTimeStamp() + "] \n");
		}
	}
	
	public void WriteErrorsToFile()
	{
		try
		{
			File file = new File(ErrorFile + getTimeStamp() );
 
			if (!file.exists()) 
			{
				file.createNewFile();
			}
 
			FileWriter fw = new FileWriter( file.getAbsoluteFile() );
			BufferedWriter bw = new BufferedWriter(fw);
			
			for( int i = 0; i < TotalErrorCount; i++ )
			{
				bw.write( "\n Error Number : [" + i + "]" );
				bw.write( "\n Error Code   : [" + Integer.toString(ErrorInfoList[i].ixiErrCode) + "]" );
				bw.write( "\n Engine State : [" + ErrorInfoList[i].ixiEngineState + "]" );
				bw.write( "\n Report       :\n\n " + ErrorInfoList[i].ixiErrReport );
				bw.write( "\n\n [" + getTimeStamp() + "] \n");
			}
			bw.close(); 
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
}
