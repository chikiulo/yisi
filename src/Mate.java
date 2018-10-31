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
import se.lth.cs.srl.util.FileExistenceVerifier;
import se.lth.cs.srl.CompletePipeline;
import se.lth.cs.srl.languages.Language;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;

public class Mate {
   protected CompletePipeline pipeline = null;

   public String init(String lang,
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
         CompletePipelineCMDLineOptions options = new CompletePipelineCMDLineOptions();
         options.parseCmdLineArgs(args);
         String error = FileExistenceVerifier.verifyCompletePipelineAllNecessaryModelFiles(options);
         if (error != null){
            result += error + "\n";
         } else {
            pipeline = CompletePipeline.getCompletePipeline(options);
         }
      } catch (Exception e){
         result += e.getMessage();
      }
      return result;
   }

   public String parse(String sentence) {
      String result = null;
      try {
         result = pipeline.parse(sentence).toString();
      } catch (Exception e) {
         e.printStackTrace();
         System.err.println(sentence);
      } 
      return result;
   }
}
