package ixi;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
 
public class ixiCopyDirectory
{
	
	public static boolean doCopy( String SourceDir, String DestinationDir )
	{
		File Source = new File(SourceDir);
    	File Destination = new File(DestinationDir);
    	
    	if( !Source.exists() )
    	{
       		return false;
        }
    	else
    	{
           try
           {
        	ixiCopy(Source, Destination);
           }
           catch
           ( IOException e )
           {
        	return false;
           }
        }
    	return true;
	}
	
	public static void ixiCopy(File Source, File Destination) throws IOException
	{
	    	if(Source.isDirectory())
	    	{
	    		if(!Destination.exists())
	    		{
	    			Destination.mkdir();
	    		}
	    		
	    		String files[] = Source.list();
	 
	    		for (String file : files)
	    		{
	    			File sourceFile = new File(Source, file);
	    			File destFile = new File(Destination, file);

	    			ixiCopy(sourceFile,destFile);
	    		}
	    	}
	    	else
	    	{
    	        int length;
    	        byte[] buffer = new byte[1024];
	    		InputStream in = new FileInputStream(Source);
	    	    OutputStream out = new FileOutputStream(Destination); 
	 
    	        while ( ( length = in.read( buffer ) ) > 0 )
    	        {
    	    	   out.write(buffer, 0, length);
    	        }
 
    	        in.close();
    	        out.close();
	    	}
	    }
}
