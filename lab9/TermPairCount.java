/* File:     TermPairCount.java
 * Authors: Ian Myers, Logan Tjossem
 * Purpose:  implement a MapReduce system with Hadoop cluster
 */
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Collections;
import java.util.ArrayList;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
public class TermPairCount {
    public static class TokenizerMapper
    extends Mapper<Object, Text, Text, IntWritable>{
        private final static IntWritable one = new IntWritable(1);
        private Text word = new Text();
        public void map(Object key, Text l, Context context) throws IOException, InterruptedException {
            ArrayList<String> A = new ArrayList<>();
            StringTokenizer itr = new StringTokenizer(l.toString());
            String term;
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
        public static void main(String[] args) throws Exception {
            Configuration conf = new Configuration();
            Job job = Job.getInstance(conf, "Term pair count");
            job.setJarByClass(TermPairCount.class);
            job.setMapperClass(TokenizerMapper.class);
            job.setCombinerClass(IntSumReducer.class);
            job.setReducerClass(IntSumReducer.class);
            job.setOutputKeyClass(Text.class);
            job.setOutputValueClass(IntWritable.class);
            FileInputFormat.addInputPath(job, new Path(args[0]));
            FileOutputFormat.setOutputPath(job, new Path(args[1]));
            System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}