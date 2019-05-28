/**
 * @file Mate.java
 * @brief Bridging class for MATE
 * 
 * @author Jackie Lo
 *
 * Multilingual Text Processing / Traitement multilingue de textes
 * Digital Technologies Research Centre / Centre de recherche en technologies numériques
 * National Research Council Canada / Conseil national de recherches Canada
 * Copyright 2018, Her Majesty in Right of Canada /
 * Copyright 2018, Sa Majeste la Reine du Chef du Canada
 */

package yisi;
import se.lth.cs.srl.corpus.Sentence;
import se.lth.cs.srl.options.CompletePipelineCMDLineOptions;
import se.lth.cs.srl.options.FullPipelineOptions;
import se.lth.cs.srl.util.FileExistenceVerifier;
import se.lth.cs.srl.CompletePipeline;
import se.lth.cs.srl.languages.Language;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.net.URL;
import java.net.URLClassLoader;
import java.lang.Class;
import java.lang.reflect.Method;

public class Mate {
   // protected CompletePipeline pipeline = null;
   protected Object pipeline = null;
   protected URLClassLoader classLoader = null;
   Class<?> class_CompletePipeline = null;

   public String init(String mate_jars,
                      String lang,
                      boolean rerank,
                      boolean hybrid,
                      String token,
                      String morph,
                      String lemma,
                      String parser,
                      String tagger,
                      String srl) {
      String result= new String();
      try {
         System.setOut(System.err);

         // Each Mate object has it's own class loader with it's own "classpath".
         String[] mateJars = mate_jars.split(":");
         URL[] urls = new URL[mateJars.length];
         for (int i = 0; i < mateJars.length; ++i) {
            urls[i] = new File(mateJars[i]).toURI().toURL();
//            System.err.println("URL: " + urls[i]);
         }
         classLoader = new URLClassLoader(urls, Thread.currentThread().getContextClassLoader());

         ArrayList<String> argsBuilder = new ArrayList<String>();
         argsBuilder.add(lang);
         argsBuilder.add("-tokenize");
         if (rerank){
            argsBuilder.add("-reranker");
         }
         if (hybrid) {
            argsBuilder.add("-hybrid");
         }
         if (! token.isEmpty()){
            argsBuilder.add("-token");
            argsBuilder.add(token);
         }
         if (! morph.isEmpty()){
            argsBuilder.add("-morph");
            argsBuilder.add(morph);
         }
         if (! lemma.isEmpty()){
            argsBuilder.add("-lemma");
            argsBuilder.add(lemma);
         }
         if (! tagger.isEmpty()){
            argsBuilder.add("-tagger");
            argsBuilder.add(tagger);
         }
         if (! parser.isEmpty()){
            argsBuilder.add("-parser");
            argsBuilder.add(parser);
         }
         if (! srl.isEmpty()){
            argsBuilder.add("-srl");
            argsBuilder.add(srl);
         }
         String[] args = new String[argsBuilder.size()];
         argsBuilder.toArray(args);
         System.err.println(java.util.Arrays.toString(args));

         Class<?> class_FullPipelineOptions = classLoader.loadClass("se.lth.cs.srl.options.FullPipelineOptions");
         // CompletePipelineCMDLineOptions options = new CompletePipelineCMDLineOptions();
         Class<?> class_CompletePipelineCMDLineOptions = classLoader.loadClass("se.lth.cs.srl.options.CompletePipelineCMDLineOptions");
         Object options = class_CompletePipelineCMDLineOptions.newInstance();
         // options.parseCmdLineArgs(args);
         Method method_parseCmdLineArgs = class_CompletePipelineCMDLineOptions.getMethod("parseCmdLineArgs", String[].class);
//         System.err.println("Got Method " + method_parseCmdLineArgs);
         method_parseCmdLineArgs.invoke(options, (Object)args);

         // String error = FileExistenceVerifier.verifyCompletePipelineAllNecessaryModelFiles(options);
         Class<?> class_FileExistenceVerifier = classLoader.loadClass("se.lth.cs.srl.util.FileExistenceVerifier");
         Method method_verifyCompletePipelineAllNecessaryModelFiles = 
               class_FileExistenceVerifier.getMethod("verifyCompletePipelineAllNecessaryModelFiles", class_FullPipelineOptions);
//         System.err.println("Got Method " + method_verifyCompletePipelineAllNecessaryModelFiles);
         String error = (String) method_verifyCompletePipelineAllNecessaryModelFiles.invoke(null, options);

         if (error != null){
            result += error + "\n";
         } else {
            // pipeline = CompletePipeline.getCompletePipeline(options);
            class_CompletePipeline = classLoader.loadClass("se.lth.cs.srl.CompletePipeline");
            Method method_getCompletePipeline = class_CompletePipeline.getMethod("getCompletePipeline", class_FullPipelineOptions);
//            System.err.println("Got Method " + method_getCompletePipeline);
            pipeline = method_getCompletePipeline.invoke(null, options);
         }
      } catch (Exception e){
         result += e.getMessage();
      }
      return result;
   }

   public String parse(String sentence) {
      String result = null;
      try {
         // result = pipeline.parse(sentence).toString();
         Method method_parse = class_CompletePipeline.getMethod("parse", String.class);
//         System.err.println("Got Method " + method_parse);
         result = method_parse.invoke(pipeline, sentence).toString();
      } catch (Exception e) {
         e.printStackTrace();
         System.err.println(sentence);
      } 
      return result;
   }
}
