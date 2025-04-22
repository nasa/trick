package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.List;

import javax.swing.*;

import java.nio.file.Path;
import java.nio.file.Paths;

/**
* Class JobPerf is an application that renders time-line data from a Trick based
  simulation. It also generates run-time statistics reports for the simulation
  jobs. It can be run with or without a GUI.
*/
public class JobPerf {
    private ArrayList<JobExecutionEvent> jobExecEvtList;
    private FrameRecord[] frameArray;
    JobStats jobStats;

    /**
     * Constructor
     * @param args the command line arguments.
     */
    public JobPerf( String[] args ) {
        TraceViewWindow traceViewWindow;
        boolean interactive = true;
        boolean printReport = false;
        JobStats.SortCriterion sortOrder = JobStats.SortCriterion.MEAN;
        String timeLineFileName = "in.csv";

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-h" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                case "-x" :
                case "--nogui" : {
                    interactive = false;
                    printReport = true;
                } break;
                case "-p" :
                case "--report" : {
                    printReport = true;
                } break;
                case "-s0" :
                case "--sort=id" : {
                    sortOrder = JobStats.SortCriterion.ID;
                } break;
                case "-s1" :
                case "--sort=mean" : {
                    sortOrder = JobStats.SortCriterion.MEAN;
                } break;
                case "-s2" :
                case "--sort=stddev" : {
                    sortOrder = JobStats.SortCriterion.STDDEV;
                } break;
                case "-s3" :
                case "--sort=max" : {
                    sortOrder = JobStats.SortCriterion.MAX;
                } break;
                case "-s4" :
                case "--sort=min" : {
                    sortOrder = JobStats.SortCriterion.MIN;
                } break;
                default : {
                    timeLineFileName = args[ii];
                } break;
            } //switch
            ++ii;
        } // while

        // All files shall be in the same directory as the timeline file.
        String filesDir = Paths.get(timeLineFileName).toAbsolutePath().getParent().toString();

        // ----------------------------------------------------------------------------------
        // Create the JobSpecificationMap from information extracted from the S_job_execution
        // file, that should be in the same directory as the time-line file.
        // ----------------------------------------------------------------------------------
        File s_job_execution_file = new File( filesDir + "/S_job_execution" );
        JobSpecificationMap jobSpecificationMap = null;
        try {
            jobSpecificationMap = new JobSpecificationMap( s_job_execution_file );
        } catch ( java.io.FileNotFoundException e ) {
            System.out.println("File \"" + s_job_execution_file.toString() + "\" not found.\n");
            System.exit(0);
        } catch ( java.io.IOException e ) {
            System.out.println("IO Exception while attempting to read " + s_job_execution_file.toString() + ".\n");
            System.exit(0);
        }

        // -----------------------------------------------------------------------------------
        // Create JobExecutionEvent list from the time-line file, and the jobSpecificationMap
        // -----------------------------------------------------------------------------------
        jobExecEvtList = getJobExecutionEventList(timeLineFileName, jobSpecificationMap);

        // ---------------------------------------------------------------------------
        // Create JobStats from the JobExecutionEvent list and the jobSpecificationMap.
        // ---------------------------------------------------------------------------
        jobStats = new JobStats(jobExecEvtList, jobSpecificationMap);

        // --------------------------------------------------
        // Create Frame Array from the JobExecutionEvent list.
        // --------------------------------------------------
        boolean wasTOF = false;
        boolean wasEOF = false;

        List<FrameRecord> frameList = new ArrayList<FrameRecord>();
        FrameRecord frameRecord = new FrameRecord();

        for (JobExecutionEvent jobExec : jobExecEvtList ) {

            if ((!wasTOF && jobExec.isTOF) || ( wasEOF && !jobExec.isEOF )) {

                // Wrap up the previous frame record.
                    frameRecord.stop = jobExec.start;
                    frameRecord.CalculateJobContainment();
                    // frameRecord.SortByStartTime();
                    frameList.add(frameRecord);

                    // Start a new frame record.
                    frameRecord = new FrameRecord();
                    frameRecord.start = jobExec.start;
            }
            frameRecord.jobEvents.add(jobExec);

            wasTOF = jobExec.isTOF;
            wasEOF = jobExec.isEOF;
        }

        frameArray = frameList.toArray( new FrameRecord[ frameList.size() ]);

        // CALC AVERAGE FRAME SIZE
        double sum = 0.0;
        for (int n=1; n < frameArray.length; n++) {
            sum += frameArray[n].getDuration();
        }
        double frameSizeAverage = sum/(frameArray.length-1);

        // ===================
        // DO DATA TESTS HERE:
        // ===================
        // TEST: Absence of TOF and EOF jobs indicates a problem. Was realtime enabled
        // when your timeline data was collected?
        if (frameArray.length <= 1) {
            System.out.println(
            "WARNING: Fewer than two frames were found.\n"+
            "Are you sure real-time was enabled when your timeline data was collected?");
        }

        for (int frameNumber=0 ; frameNumber < frameArray.length ; frameNumber++ ) {
            FrameRecord frame = frameArray[frameNumber];

            for (JobExecutionEvent jobExec : frame.jobEvents) {

                // TEST: Job start times equal to zero, beyond the first frame, indicate a problem.
                if ( frameNumber>0 && jobExec.start==0 ) {
                    System.out.println("WARNING: Job " + jobExec.id + " in frame " + frameNumber + " has a start time of zero.");
                }
                
                // TEST: Job start times greater than the stop time indicates a problem.
                if ( jobExec.start >  jobExec.stop) {
                    System.out.println("ERROR: Job " + jobExec.id + " in frame " + frameNumber + " has a start time greater than stop time.");
                }

                // TODO: TEST: Check that each of the IDs in the timeline are found in the
                //   S_job_execution file. The greater the number of missing IDs,
                //   the greater is the problem. Report the number of missing IDs.
            }
 
            double frameSizeRatio = frame.getDuration() / frameSizeAverage;
            int percentage = (int)(frameSizeRatio * 100);

            // TEST: A frame duration significantly above the average could be a problem.
            if (percentage > 110) {
                System.out.println("WARNING: Size of frame " + frameNumber + " is > "+ percentage +"% of the average frame size.");
                System.out.println("Could be a frame over-run, or perhaps the time-line was collected with itimers enabled." );
            }
            // TEST: A frame duration significantly below the average could be a problem.
            if ((frameNumber !=0) && (percentage < 90)) {
                System.out.println("WARNING: Size of frame " + frameNumber + " is < "+ percentage +"% of the average frame size.");
                System.out.println("Perhaps the sim user-code is calling a \"start-of-frame\" or \"end-of-frame\" job directly?" );
            }
        }

        if (printReport) {
            if (sortOrder == JobStats.SortCriterion.ID ) jobStats.SortByID();
            if (sortOrder == JobStats.SortCriterion.MEAN ) jobStats.SortByMeanValue();
            if (sortOrder == JobStats.SortCriterion.STDDEV ) jobStats.SortByStdDev();
            if (sortOrder == JobStats.SortCriterion.MAX ) jobStats.SortByMaxValue();
            if (sortOrder == JobStats.SortCriterion.MIN ) jobStats.SortByMinValue();
            jobStats.write();
        }
        if (interactive) {
            traceViewWindow = new TraceViewWindow(filesDir, frameArray, jobStats, jobSpecificationMap);
        }
    }

    /**
     * Print the usage instructions to the terminal.
     */
    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
            + "usage: trick-jperf [options] <file-name>\n\n"
            + "options: \n"
            + "-h, --help\n"
            + "    Print this help text and exit.\n"
            + "-x, --nogui\n"
            + "    Don't run as a GUI application. Command line only.\n"
            + "-p, --report\n"
            + "    Write sorted job statics report to the terminal.\n"
            + "-s0, --sort=id\n"
            + "    Sort job statistics by identifier.\n"
            + "-s1, --sort=mean   [default]\n"
            + "    Sort job statistics by mean duration.\n"
            + "-s2, --sort=stddev\n"
            + "    Sort job statistics by standard deviation of duration.\n"
            + "-s3, --sort=min\n"
            + "    Sort job statistics by minimum duration.\n"
            + "-s4, --sort=max\n"
            + "    Sort job statistics by maximum duration.\n"
            + "----------------------------------------------------------------------\n"
          );
    }

    /**
     * Read the timeline file, resulting in a ArrayList<JobExecutionEvent>.
     */
    private ArrayList<JobExecutionEvent> getJobExecutionEventList( String fileName,
                                                                   JobSpecificationMap jobSpecificationMap ) {
        String line;
        String field[];

        ArrayList<JobExecutionEvent> jobExecEvtList = new ArrayList<JobExecutionEvent>();
        try {
            BufferedReader in = new BufferedReader( new FileReader(fileName) );

            // Strip the header line off the CSV file.
            line = in.readLine();

            // Iterate through and process each of the data lines.
            while( (line = in.readLine()) !=null) {
                 boolean isTOF = false;
                 boolean isEOF = false;
                 field   = line.split(",");

                 String id    = field[0].trim();
                 JobSpecification jobSpec = jobSpecificationMap.getJobSpecification(id);
                 if (jobSpec != null) {
                     if (jobSpec.jobClass.equals("top_of_frame")) {
                         isTOF = true;
                     } else if (jobSpec.jobClass.equals("end_of_frame")) {
                         isEOF = true;
                     }
                 } 
                 double start = Double.parseDouble( field[1]);
                 double stop  = Double.parseDouble( field[2]);
                 if (start < stop) {
                     JobExecutionEvent evt = new JobExecutionEvent(id, isTOF, isEOF, start, stop);
                     jobExecEvtList.add( evt);
                 }
             }
         } catch ( java.io.FileNotFoundException e ) {
             System.out.println("File \"" + fileName + "\" not found.\n");
             System.exit(0);
         } catch ( java.io.IOException e ) {
             System.out.println("IO Exception.\n");
             System.exit(0);
         }
         return jobExecEvtList;
    }

    /**
     * Entry point for the Java application.
     */
    public static void main(String[] args) {
        JobPerf jobPerf = new JobPerf( args );
    }
}
