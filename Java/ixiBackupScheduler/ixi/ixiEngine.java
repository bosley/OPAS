package ixi;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;

public class ixiEngine
{
	// Engine State
	enum State
	{
		IDLE,
		INITIATING,
		BACKUP_LOAD_WORKING,
		CONFIG_LOAD_WORKING,
		CHECKING_SCHEDULE,
		CREATING_JOB,
		BACKINGUP,
	}
	
	// Job Structure
	class Backup
	{
		public String  Title;
		public String  Source;
		public String  Destination;
		public Date    NextUpdate;
		public int	   UpdateUnit;			// 1:D | 2:H | 3:W | 4:M
		public int	   UpdateIncrement;
		public int 	   BackupId;
	}
	
	public class Config
	{
		/*		Window Configuration			*/
		public int doSetup;
		public int doShowMainDisplay;
		public int doShowBackupWindow;
		
		/*		Error Handle Configuration		*/
		public int doErrorPopup;
		public int doErrorConsole;
		public int doErrorFile;
	}
	
	public State CurrentState;	
	public Backup BackupList[];
	public Config CurrentConfig; 
	public ixiErr ixiErrorHandler;	
	public ixiCopyDirectory ixiCopier;
	public SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd.HH-mm-ss");
	
	public int backupCount;
	public int backupMaxCount;
	
    public static Calendar Cal;
	public static Date CurrentDate;
	public static String ErrorFile;
	public static String configFile;
	public static String scheduleFile;
	public static String workingDir;
	
	public boolean getSource = false;
	public boolean getDestination = false;
	
	public String pathTest = null;

	public String HostOs = System.getProperty("os.name");
	
	private HashMap<Integer, String> EngineErrCode = new HashMap<Integer, String>() ;

	public ixiEngine()
	{
		// Error Reporting Information
		EngineErrCode.put(1, "NonNumeric Entered");
		EngineErrCode.put(2, "loadConfigFile()");
		EngineErrCode.put(3, "loadScheduleFile()");
		EngineErrCode.put(4, "Invalid Update Unit");
		EngineErrCode.put(5, "checkSchedule()");
		EngineErrCode.put(6, "Error Performing Backup : Source Not Found ");
		EngineErrCode.put(7, "Max Backup Count Reached");
		EngineErrCode.put(8, "Unknown OS");
		
		// Setup Error Reporter
		ixiErrorHandler = new ixiErr();
		
		setWorkingDir();
		
		// Make this work on windows..
		if ( HostOs.contains("Linux") )
		{
			configFile = workingDir + "/src/ixi/info/config";
			scheduleFile = workingDir + "/src/ixi/info/schedule";
			ErrorFile = workingDir + "/src/ixi/info/log/ErrorLog/Report_";
		}
		else if ( HostOs.contains("Windows") )
		{
			configFile = workingDir + "C:\\Program Files\\ixi\\info\\config";
			scheduleFile = workingDir + "C:\\Program Files\\ixi\\info\\schedule";
			ErrorFile = workingDir + "C:\\Program Files\\ixi\\info\\log\\ErrorLog\\Report_";
		}
		else
		{
			ixiErrorHandler.ReportError(9, EngineErrCode.get(8), CurrentState.toString());
			System.exit(0);
		}
		ixiErrorHandler.ErrorFile = ErrorFile;

		backupCount = 0;
		backupMaxCount = 20;
		CurrentDate = new Date();
		CurrentConfig = new Config();
		BackupList = new Backup[ backupMaxCount ];
		
		Cal = Calendar.getInstance();
		Cal.setTime(CurrentDate);
		
		CurrentState = State.INITIATING;
		
		/*		Load configuration and BackupList		*/
		try
		{
			loadConfigFile();
			loadScheduleFile();
			CurrentState = State.IDLE;
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
		
		checkSchedule();	// Starts Today's Backups	
	}
	
	/*			Private Methods							*/
	
	private void setWorkingDir()
	{
		File f = null;
		workingDir = "";
		f = new File(workingDir);
		workingDir = f.getAbsolutePath();
	}
	
	@SuppressWarnings("unused")
	public boolean isNumeric(String s)  
	{  
		try  
		{  
			double d = Double.parseDouble(s);  
		}  
		catch(NumberFormatException nfe)  
		{  
			ixiErrorHandler.ReportError(8, EngineErrCode.get(1), CurrentState.toString());
			return false;  
		}  
		return true;  
	}
	
	private void loadConfigFile() throws FileNotFoundException
	{
		String line = null;
		FileInputStream cfile = new FileInputStream(configFile);
		BufferedReader br = new BufferedReader(new InputStreamReader(cfile));
		
		// Open the file
		try
		{
			while ((line = br.readLine()) != null)
			{

				String[] exploded = line.split("=");
				exploded[0] = exploded[0].replaceAll("\\s","");

				/*			Window Setting					*/
				if( exploded[0].equals("@doSetup") )
				{
					CurrentConfig.doSetup = Integer.parseInt( exploded[1].replaceAll("\\s","") );
				}
				if( exploded[0].equals("@doShowMainDisplay") )
				{
					CurrentConfig.doShowMainDisplay = Integer.parseInt( exploded[1].replaceAll("\\s","") );
				}
				if( exploded[0].equals("@doShowBackupWindow") )
				{
					CurrentConfig.doShowBackupWindow = Integer.parseInt( exploded[1].replaceAll("\\s","")  );
				}
				
				/*			Error Setting					*/
				if( exploded[0].equals("@doErrorConsole") )
				{
					CurrentConfig.doErrorConsole = Integer.parseInt( exploded[1].replaceAll("\\s","") );
					ixiErrorHandler.WriteErrorToConsole = CurrentConfig.doErrorConsole;
				}
				if( exploded[0].equals("@doErrorFile") )
				{
					CurrentConfig.doErrorFile = Integer.parseInt( exploded[1].replaceAll("\\s","") );
					ixiErrorHandler.WriteErrorToFile = CurrentConfig.doErrorFile;
				}
				if( exploded[0].equals("@doErrorPopup") )
				{
					CurrentConfig.doErrorPopup = Integer.parseInt( exploded[1].replaceAll("\\s","") );
					ixiErrorHandler.WriteErrorToPopup = CurrentConfig.doErrorPopup;
				}
			}
		}
		catch (IOException e)
		{
			ixiErrorHandler.ReportError( 0, EngineErrCode.get(2), CurrentState.toString() );
			e.printStackTrace();
		}
	 
		// Close the file
		try
		{
			br.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	} // END LOAD CONFIG
	
	private void loadScheduleFile() throws FileNotFoundException
	{
		// Clear list before building
		for( int i = 0; i < backupMaxCount; i++ )
		{
			BackupList[i] = null;
		}
		
		CurrentState = State.BACKUP_LOAD_WORKING;
		
		String line = null;
		int lineIndex = 0, lineLength;
		FileInputStream cfile = new FileInputStream(scheduleFile);
		BufferedReader br = new BufferedReader(new InputStreamReader(cfile));
		
		// Open the file
		try
		{
			while ((line = br.readLine()) != null)
			{
				
				String[] exploded = line.split(",");
				exploded[0] = exploded[0].replaceAll("\\s","");
				lineLength = exploded.length;
				
				BackupList[ backupCount ] = new Backup();
				BackupList[ backupCount ].BackupId = backupCount;
				
				for( lineIndex = 0; lineIndex < lineLength; lineIndex++)
				{
					switch( lineIndex )
					{
					case 0:
						BackupList[ backupCount ].Title =  exploded[lineIndex] ;
						break;
						
					case 1:
						BackupList[ backupCount ].Source =  exploded[lineIndex] ;
						break;
						
					case 2:
						BackupList[ backupCount ].Destination =  exploded[lineIndex] ;
						break;
						
					case 3:
						try 
						{
							BackupList[ backupCount ].NextUpdate = dateFormatter.parse( exploded[lineIndex]);
						} 
						catch (ParseException e)
						{
							e.printStackTrace();
						}
						break;
						
					case 4:
						BackupList[ backupCount ].UpdateIncrement =  Integer.parseInt(exploded[lineIndex] ) ;
						break;
						
					case 5:
						BackupList[ backupCount ].UpdateUnit =  Integer.parseInt(exploded[lineIndex] ) ;
						break;
						
					default:
						ixiErrorHandler.ReportError( 0, EngineErrCode.get(3), CurrentState.toString() );
						System.exit(0);
						break;
					}
				}
				backupCount++;
			}
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	 
		// Close the file
		try
		{
			br.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}		
	}
	
	private void checkSchedule()
	{
		for( int bi = 0; bi < backupCount; bi++ )
		{
			CurrentState = State.CHECKING_SCHEDULE;
			
			if ( CurrentDate.equals(BackupList[bi].NextUpdate) || CurrentDate.after(BackupList[bi].NextUpdate) )
			{	
				 if ( initiateBackup(bi) )
				 {
					 
					 switch( BackupList[bi].UpdateUnit )
					 {
					 case 0:
						 	ixiErrorHandler.ReportError( 1, EngineErrCode.get(4), CurrentState.toString() );
						 	System.exit(0);
						 break;
					 
					 case 1:
						 // Daily Update
							Cal.add(Calendar.DATE, BackupList[bi].UpdateIncrement); 
							BackupList[bi].NextUpdate = Cal.getTime();
						 break;
						 
					 case 2:
						 // Hourly Update
						 	Cal.add(Calendar.HOUR_OF_DAY, BackupList[bi].UpdateIncrement); 
						 	BackupList[bi].NextUpdate = Cal.getTime();
						 break;
						 
					 case 3:
						 // Weekly Update
						 	Cal.add(Calendar.WEEK_OF_MONTH, BackupList[bi].UpdateIncrement); 
						 	BackupList[bi].NextUpdate = Cal.getTime();
						 break;
						 
					 case 4:
						 // Monthly Update
						 	Cal.add(Calendar.MONTH, BackupList[bi].UpdateIncrement); 
						 	BackupList[bi].NextUpdate = Cal.getTime();
						 break;
						 
					default :
						 	ixiErrorHandler.ReportError( 1, EngineErrCode.get(5), CurrentState.toString() );
						 	System.exit(0);
						break;
					 }

					// Reset Calendar
					Cal.setTime(CurrentDate);
				 }		
				 else
				 {
					 ixiErrorHandler.ReportError( 0, EngineErrCode.get(6) + bi, CurrentState.toString() );
				 }
			}
		}
	}
	
	private boolean initiateBackup( int backupID )
	{
		CurrentState = State.BACKINGUP;

		return  ixiCopyDirectory.doCopy( BackupList[backupID].Source, BackupList[backupID].Destination );
	}
		
	/*			Update Files							*/
	
	public void updateConfigFile()
	{
		try
		{
			File file = new File(configFile);
 
			if (!file.exists()) 
			{
				file.createNewFile();
			}
 
			FileWriter fw = new FileWriter( file.getAbsoluteFile() );
			BufferedWriter bw = new BufferedWriter(fw);
			
			bw.write("# Window Display Settings \n");
			bw.write("@doSetup = " + String.valueOf(CurrentConfig.doSetup) + "\n");
			bw.write("@doShowMainDisplay = " + String.valueOf(CurrentConfig.doShowMainDisplay) + "\n");
			bw.write("@doShowBackupWindow = " + String.valueOf(CurrentConfig.doShowBackupWindow) + "\n");
			bw.write("# Error Report Settings \n");
			bw.write("@doErrorPopup = " + String.valueOf(CurrentConfig.doErrorPopup) + "\n");
			bw.write("@doErrorConsole = " + String.valueOf(CurrentConfig.doErrorConsole) + "\n");
			bw.write("@doErrorFile = " + String.valueOf(CurrentConfig.doErrorFile) + "\n");

			bw.close(); 
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	public void updateScheduleFile()
	{		
		try
		{
			File file = new File(scheduleFile);
 
			if (!file.exists()) 
			{
				file.createNewFile();
			}
 
			FileWriter fw = new FileWriter( file.getAbsoluteFile() );
			BufferedWriter bw = new BufferedWriter(fw);
			
			for( int bi = 0; bi < backupCount; bi++ )
			{
				if ( !"[$!~REMV_***]".equals( BackupList[bi].Title) )
				{
					bw.write( BackupList[bi].Title + ",");
					bw.write( BackupList[bi].Source + ",");
					bw.write( BackupList[bi].Destination + ",");
					bw.write( dateFormatter.format(BackupList[bi].NextUpdate) + ",");
					bw.write( BackupList[bi].UpdateIncrement + ",");
					bw.write( BackupList[bi].UpdateUnit + "\n");
				}
			}
			
			bw.close(); 
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
		
		// Clear the current backup list
		Arrays.fill( BackupList, null );
		backupCount = 0;
		
		// Reload with new information
		try
		{
			loadScheduleFile();
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
	}

	/*			Data Retrieval and Setting 				*/
	
	public int newBackup()
	{
		CurrentState = State.CREATING_JOB;
		if( backupCount >= backupMaxCount )
		{
			ixiErrorHandler.ReportError(9, EngineErrCode.get(7), CurrentState.toString() );
		}
		BackupList[backupCount] = new Backup();
		return backupCount++;
	}
	
	public void setBackupByID( int ID, Backup B)
	{
		if ( ID > backupCount )
		{
			backupCount = ID;
		}
		BackupList[ID] = B;
	}
	
	public Backup getBackupByID(int ID)
	{
		return BackupList[ ID ];
	}
	
	public void setConfiguration( Config C )
	{
		CurrentConfig = C;
	}
	
	public Config getConfiguration()
	{
		return CurrentConfig;
	}

	public String[] getListOfBackups()
	{
		String[] lob = new String[backupCount];
		for( int i = 0; i < backupCount; i++ )
		{
			lob[i] = BackupList[i].BackupId + ": " + BackupList[i].Title;
		}
		return lob;
	}	
	
	
}//
