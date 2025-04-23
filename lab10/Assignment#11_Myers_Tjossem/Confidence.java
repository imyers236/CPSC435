/* File:     Confidence.java
 * Authors: Ian Myers, Logan Tjossem
 * Purpose:  implement two MapReduce systems with Hadoop cluster
 */
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Collections;
import java.util.ArrayList;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Set;
import java.util.List;
import java.io.BufferedReader;
import java.util.HashSet;
import java.net.URI;
import java.io.FileReader;
import java.util.StringTokenizer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.util.GenericOptionsParser;
import org.apache.hadoop.util.StringUtils;
public class Confidence {
    public static class TokenizerMapper
    extends Mapper<Object, Text, Text, IntWritable>{
        private final static IntWritable one = new IntWritable(1);
        private Text word = new Text();
        private boolean caseSensitive;
        private Set<String> patternsToSkip = new HashSet<String>();
        private Configuration conf;
        private BufferedReader fis;

        @Override
        public void setup(Context context) throws IOException,
        InterruptedException {
            conf = context.getConfiguration();
            caseSensitive = conf.getBoolean("wordcount.case.sensitive", true);
            if (conf.getBoolean("wordcount.skip.patterns", false)) {
                URI[] patternsURIs = Job.getInstance(conf).getCacheFiles();
                for (URI patternsURI : patternsURIs) {
                    Path patternsPath = new Path(patternsURI.getPath());
                    String patternsFileName = patternsPath.getName().toString();
                    parseSkipFile(patternsFileName);
                }
            }
        }

        private void parseSkipFile(String fileName) {
            try {
                fis = new BufferedReader(new FileReader(fileName));
                String pattern = null;
                while ((pattern = fis.readLine()) != null) {
                    patternsToSkip.add(pattern);
                }
            } catch (IOException ioe) {
                System.err.println("Caught exception while parsing the cached file '"
                + StringUtils.stringifyException(ioe));
            }
        }


        @Override
        public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            String term = (caseSensitive) ?
                value.toString() : value.toString().toLowerCase();
            for (String pattern : patternsToSkip) {
                term = term.replaceAll(pattern, "");
            }
            ArrayList<String> A = new ArrayList<>();
            StringTokenizer itr = new StringTokenizer(term);
            while (itr.hasMoreTokens()) {
                // This adds all new words to the arraylist
                term = itr.nextToken();
                if(!A.contains(term))
                    A.add(term);
            }
            // sorts all words in arraylist
            Collections.sort(A);
            for (int i = 0; i < A.size(); i++)
            {
                // adds words to context
                word.set(A.get(i));
                context.write(word, one);
                for (int j = i + 1; j < A.size(); j++)
                {
                    // adds pairs to context
                    word.set(A.get(i)+":"+A.get(j));
                    context.write(word, one);
                }
            }
            
        }
        }
        public static class IntSumReducer
        extends Reducer<Text,IntWritable,Text,IntWritable> {
            private IntWritable result = new IntWritable();
            public void reduce(Text key, Iterable<IntWritable> values,
            Context context
            ) throws IOException, InterruptedException {
                int sum = 0;
                for (IntWritable val : values) {
                sum += val.get();
                }
                result.set(sum);
                context.write(key, result);
            }
        }
        public static class MapTwo
        extends Mapper<Object, Text, Text, Text>{
            private Text word = new Text();
            private Text wordTwo = new Text();
            @Override
            public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
                StringTokenizer itr = new StringTokenizer(value.toString());
                String term;
                String keyWord1;
                String keyWord2;
                String valStr;
                int colonIndex;
                while (itr.hasMoreTokens()) {
                    term = itr.nextToken();
                    if(term.contains(":"))
                    {
                        // find index of colon
                        colonIndex = term.indexOf(":");
                        // separate key words
                        keyWord1 = term.substring(0, colonIndex);
                        keyWord2 = term.substring(colonIndex + 1, term.length());
                        // set keyword1 and text with number together
                        word.set(keyWord1);
                        valStr = term;
                        term = itr.nextToken();
                        wordTwo.set(valStr+" "+term);
                        context.write(word, wordTwo);
                        // set keyword2 and text with number together
                        word.set(keyWord2);
                        wordTwo.set(keyWord2 + ":" + keyWord1 + " " + term);
                        context.write(word, wordTwo);
                    }
                    else
                    {
                        // Just word and number
                        // Sets both and writes to context
                        word.set(term);
                        term = itr.nextToken();
                        wordTwo.set(term);
                        context.write(word, wordTwo);
                    }   
                }
                        
            }
        }

        public static class ReduceTwo
        extends Reducer<Text, Text, Text, Text> {
            private FloatWritable result = new FloatWritable();
            private Text word = new Text();
            private Text wordTwo = new Text();
            @Override
            public void reduce(Text key, Iterable<Text> values,
            Context context
            ) throws IOException, InterruptedException {
                // initialization
                ArrayList<String> A = new ArrayList<>();
                float denom = 1.0f;
                float num = 1.0f;
                int colonIndex;
                int spaceIndex;
                String secondKey;
                String valueStr;
                float conf = 1.0f;
                // iterates through all values for the key
                for(Text value : values)
                {
                    valueStr = value.toString();
                    if(!valueStr.contains(":"))
                    {
                        // parse just number at denominator
                        denom = Float.parseFloat(valueStr);
                    }
                    else
                    {
                        // if term and number add to list
                        A.add(valueStr);
                    }
                }
                for(int i = 0; i < A.size(); i++)
                {
                    valueStr = A.get(i);
                    // find split between term and number
                    spaceIndex = valueStr.indexOf(" ");
                    // set term from beginning of string
                    word.set(valueStr.substring(0, spaceIndex));
                    // set number as end of string
                    num = Float.parseFloat(valueStr.substring(spaceIndex, valueStr.length()));
                    // find confidence
                    conf = num / denom;
                    // set and write to context
                    result.set(conf);
                    wordTwo.set(result.toString());
                    context.write(word, wordTwo);
                }

                
            }
        }
        
        public static void main(String[] args) throws Exception {
            Configuration conf = new Configuration();
            GenericOptionsParser optionParser = new GenericOptionsParser(conf,
            args);
            String[] remainingArgs = optionParser.getRemainingArgs();
            if ((remainingArgs.length != 4) && (remainingArgs.length != 6)) {
                System.err.println("Usage: wordcount <in> <out> [-skip skipPatternFile]");
                System.exit(2);
            }
            Job job = Job.getInstance(conf, "TermPairCount");
            job.setJarByClass(Confidence.class);
            job.setMapperClass(TokenizerMapper.class);
            job.setCombinerClass(IntSumReducer.class);
            job.setReducerClass(IntSumReducer.class);
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(IntWritable.class);
            List<String> otherArgs = new ArrayList<String>();
            for (int i=0; i < remainingArgs.length; ++i) {
                if ("-skip".equals(remainingArgs[i])) {
                    job.addCacheFile(new Path(remainingArgs[++i]).toUri());
                    job.getConfiguration().setBoolean("wordcount.skip.patterns", true);
                } else {
                    otherArgs.add(remainingArgs[i]);
                }
            }
            FileInputFormat.addInputPath(job, new Path(otherArgs.get(0)));
            FileOutputFormat.setOutputPath(job, new Path(otherArgs.get(1)));
            job.waitForCompletion(true); // first MapReduce job finishes here

            Configuration confTwo = new Configuration();
            Job job2 = Job.getInstance(confTwo, "Confidence");
            job2.setJarByClass(Confidence.class);
            job2.setMapperClass(MapTwo.class);
            job2.setReducerClass(ReduceTwo.class);
            job2.setOutputKeyClass(Text.class);
            job2.setOutputValueClass(Text.class);
            FileInputFormat.addInputPath(job2, new Path(otherArgs.get(2)));
            FileOutputFormat.setOutputPath(job2, new Path(otherArgs.get(3)));
            System.exit(job2.waitForCompletion(true) ? 0 : 1);
        }
}