#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <unordered_map>

using namespace std;

class Paper{
	public:
		vector< int > successorList;
		vector< int > predecessorList;
		vector< int > authorList;
		vector< int > affiliationList;
		int year;
		int conference;
		int journal;
		int conferenceHindex;
		int journalHindex;
};

class PageRank{
	private:
		double dampingFactor;
		double convergenceThreshold;
		double smallPositiveValue;
		vector< double > paperScoreList;
		
		int getConferenceCount(vector< Paper > &graph){
			int paperCount = graph.size();
			int maxConference = -1;
			bool isFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				int conference = graph[paper].conference;
				if(conference < 0){
					continue;
				}

				if(isFirst || maxConference < conference){
					maxConference = conference;
					isFirst = false;
				}
			}
			return maxConference + 1;
		}

		int getJournalCount(vector< Paper > &graph){
			int paperCount = graph.size();
			int maxJournal = -1;
			bool isFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				int journal = graph[paper].journal;
				if(journal < 0){
					continue;
				}

				if(isFirst || maxJournal < journal){
					maxJournal = journal;
					isFirst = false;
				}
			}
			return maxJournal + 1;
		}

		int getAuthorCount(vector< Paper > &graph){
			int paperCount = graph.size();
			int maxAuthor = -1;
			bool isFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				for(auto &author: graph[paper].authorList){
					if(isFirst || maxAuthor < author){
						maxAuthor = author;
						isFirst = false;
					}
				}
			}
			return maxAuthor + 1;
		}
		
		int getAffiliationCount(vector< Paper > &graph){
			int paperCount = graph.size();
			int maxAffiliation = -1;
			bool isFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				for(auto &affiliation: graph[paper].affiliationList){
					if(isFirst || maxAffiliation < affiliation){
						maxAffiliation = affiliation;
						isFirst = false;
					}
				}
			}
			return maxAffiliation + 1;
		}
		
		int getCurrentYear(vector< Paper > &graph){
			int paperCount = graph.size();

			int latestYear = 0;
			bool lateFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				int year = graph[paper].year;
				if(lateFirst || latestYear < year){
					latestYear = year;
					lateFirst = false;
				}
			}
			return latestYear + 1;
		}
		
		void computeConferenceWeights(vector< Paper > &graph, vector< double > &paperWeightList, vector< double > &conferenceScoreList, double &conferenceScoreMean){
			printf("\tCompute conference scores\n");
			int paperCount = graph.size();
			int conferenceCount = this -> getConferenceCount(graph);
			
			conferenceScoreList.assign(conferenceCount, 0.0);
			vector< int > conferencePaperCountList(conferenceCount, 0);
			for(int paper = 0; paper < paperCount; paper ++){
				int conference = graph[paper].conference;
				if(conference < 0){
					continue;
				}

				conferenceScoreList[conference] += paperWeightList[paper];
				conferencePaperCountList[conference] += 1;
			}

			conferenceScoreMean = 0.0;
			int conferenceEffectiveCount = 0;
			for(int conference = 0; conference < conferenceCount; conference ++){
				if(conferencePaperCountList[conference] > 0){
					conferenceScoreList[conference] /= conferencePaperCountList[conference];
				
					conferenceScoreMean += conferenceScoreList[conference];
					conferenceEffectiveCount += 1;
				}
			}
			if(conferenceEffectiveCount > 0){
				conferenceScoreMean /= conferenceEffectiveCount;
			}
		}
		
		void computeJournalWeights(vector< Paper > &graph, vector< double > &paperWeightList, vector< double > &journalScoreList, double &journalScoreMean){
			printf("\tCompute journal scores\n");
			int paperCount = graph.size();
			int journalCount = this -> getJournalCount(graph);
			
			journalScoreList.assign(journalCount, 0.0);
			vector< int > journalPaperCountList(journalCount, 0);
			for(int paper = 0; paper < paperCount; paper ++){
				int journal = graph[paper].journal;
				if(journal < 0){
					continue;
				}

				journalScoreList[journal] += paperWeightList[paper];
				journalPaperCountList[journal] += 1;
			}

			journalScoreMean = 0.0;
			int journalEffectiveCount = 0;
			for(int journal = 0; journal < journalCount; journal ++){
				if(journalPaperCountList[journal] > 0){
					journalScoreList[journal] /= journalPaperCountList[journal];
				
					journalScoreMean += journalScoreList[journal];
					journalEffectiveCount += 1;
				}
			}
			if(journalEffectiveCount > 0){
				journalScoreMean /= journalEffectiveCount;
			}
		}
		
		void computeAuthorWeights(vector< Paper > &graph, vector< double > &paperWeightList, vector< double > &authorScoreList, double &authorScoreMean){
			printf("\tCompute author scores\n");
			int paperCount = graph.size();
			int authorCount = this -> getAuthorCount(graph);
			
			authorScoreList.assign(authorCount, 0.0);
			vector< int > authorPaperCountList(authorCount, 0);
			for(int paper = 0; paper < paperCount; paper ++){
				int paperAuthorCount = graph[paper].authorList.size();
				if(paperAuthorCount > 0){
					double vote = paperWeightList[paper] / paperAuthorCount;
					for(auto &author: graph[paper].authorList){
						authorScoreList[author] += vote;
						authorPaperCountList[author] += 1;
					}
				}
			}
			
			authorScoreMean = 0.0;
			int authorEffectiveCount = 0;
			for(int author = 0; author < authorCount; author ++){
				if(authorPaperCountList[author] > 0){
					authorScoreList[author] /= authorPaperCountList[author];
				
					authorScoreMean += authorScoreList[author];
					authorEffectiveCount += 1;
				}
			}
			if(authorEffectiveCount > 0){
				authorScoreMean /= authorEffectiveCount;
			}
		}
		
		void computeAffiliationWeights(vector< Paper > &graph, vector< double > &paperWeightList, vector< double > &affiliationScoreList, double &affiliationScoreMean){
			printf("\tCompute affiliation scores\n");
			int paperCount = graph.size();
			int affiliationCount = this -> getAffiliationCount(graph);
			
			affiliationScoreList.assign(affiliationCount, 0.0);
			vector< int > affiliationPaperCountList(affiliationCount, 0);
			for(int paper = 0; paper < paperCount; paper ++){
				int paperAffiliationCount = graph[paper].affiliationList.size();
				if(paperAffiliationCount > 0){
					double vote = paperWeightList[paper] / paperAffiliationCount;
					for(auto &affiliation: graph[paper].affiliationList){
						affiliationScoreList[affiliation] += vote;
						affiliationPaperCountList[affiliation] += 1;
					}
				}
			}
			
			affiliationScoreMean = 0.0;
			int affiliationEffectiveCount = 0;
			for(int affiliation = 0; affiliation < affiliationCount; affiliation ++){
				if(affiliationPaperCountList[affiliation] > 0){
					affiliationScoreList[affiliation] /= affiliationPaperCountList[affiliation];
				
					affiliationScoreMean += affiliationScoreList[affiliation];
					affiliationEffectiveCount += 1;
				}
			}
			if(affiliationEffectiveCount > 0){
				affiliationScoreMean /= affiliationEffectiveCount;
			}
		}

		void computeInitialPaperWeights(vector< Paper > &graph, vector< double > &paperWeightList){
			int paperCount = graph.size();
			int currentYear = this -> getCurrentYear(graph);

			paperWeightList.assign(paperCount, 0.0);
			for(int paper = 0; paper < paperCount; paper ++){
				int indegree = graph[paper].predecessorList.size();
				paperWeightList[paper] = (indegree > 0)? (double)indegree / (currentYear - graph[paper].year): this -> smallPositiveValue;
			}
		}
		
		void generatePaperWeightList(vector< Paper > &graph, vector< double > &paperWeightList){
			int paperCount = graph.size();

			this -> computeInitialPaperWeights(graph, paperWeightList);
			
			vector< double > conferenceScoreList;
			double conferenceScoreMean = 0.0;
			this -> computeConferenceWeights(graph, paperWeightList, conferenceScoreList, conferenceScoreMean);
			
			vector< double > journalScoreList;
			double journalScoreMean = 0.0;
			this -> computeJournalWeights(graph, paperWeightList, journalScoreList, journalScoreMean);
			
			for(int paper = 0; paper < paperCount; paper ++){
				double conferenceScore = conferenceScoreMean;
				int conference = graph[paper].conference;
				if(conference >= 0){
					conferenceScore = conferenceScoreList[conference];
				}

				paperWeightList[paper] += conferenceScore;
			}
			
			for(int paper = 0; paper < paperCount; paper ++){
				double journalScore = journalScoreMean;
				int journal = graph[paper].journal;
				if(journal >= 0){
					journalScore = journalScoreList[journal];
				}

				paperWeightList[paper] += journalScore;
			}
			
			vector< double > authorScoreList;
			double authorScoreMean = 0.0;
			this -> computeAuthorWeights(graph, paperWeightList, authorScoreList, authorScoreMean);
			
			vector< double > affiliationScoreList;
			double affiliationScoreMean = 0.0;
			this -> computeAffiliationWeights(graph, paperWeightList, affiliationScoreList, affiliationScoreMean);
			
			for(int paper = 0; paper < paperCount; paper ++){
				double authorScore = authorScoreMean;
				int paperAuthorCount = graph[paper].authorList.size();
				if(paperAuthorCount > 0){
					authorScore = 0.0;
					for(auto &author: graph[paper].authorList){
						authorScore += authorScoreList[author];
					}
					authorScore /= paperAuthorCount;
				}
				
				paperWeightList[paper] += authorScore;
			}
			
			for(int paper = 0; paper < paperCount; paper ++){
				double affiliationScore = affiliationScoreMean;
				int paperAffiliationCount = graph[paper].affiliationList.size();
				if(paperAffiliationCount > 0){
					affiliationScore = 0.0;
					for(auto &affiliation: graph[paper].affiliationList){
						affiliationScore += affiliationScoreList[affiliation];
					}
					affiliationScore /= paperAffiliationCount;
				}
			
				paperWeightList[paper] += affiliationScore;
			}
			
		}

		void runPaperPageRank(vector< Paper > &graph){
			printf("\tDeclare variables and compute weights\n");

			int paperCount = graph.size();

			vector< double > lastPaperScoreList(paperCount, 1.0 / paperCount);

			vector< double > paperWeightList;
			this -> generatePaperWeightList(graph, paperWeightList);

			vector< double > paperSuccessorWeightSumList(paperCount, 0.0);
			for(int paper = 0; paper < paperCount; paper ++){
				for(auto &neighbor: graph[paper].successorList){
					paperSuccessorWeightSumList[paper] += paperWeightList[neighbor];
				}
			}
			
			vector< double > paperRegularizationList(paperWeightList);
			double paperWeightSum = 0.0;
			for(int paper = 0; paper < paperCount; paper ++){
				paperWeightSum += paperRegularizationList[paper];
			}
			if(paperWeightSum > 0){
				for(int paper = 0; paper < paperCount; paper ++){
					paperRegularizationList[paper] /= paperWeightSum;
				}
			}
			
			printf("\tStart algorithms\n");
			for(int iteration = 1; ; iteration ++){
				printf("\t\tIteration %d", iteration);
			
				this -> paperScoreList.assign(paperCount, 0.0);

				double zeroOutdegreePaperScore = 0.0;
				for(int paper = 0; paper < paperCount; paper ++){
					if(graph[paper].successorList.size() == 0 || paperSuccessorWeightSumList[paper] == 0.0){
						zeroOutdegreePaperScore += lastPaperScoreList[paper];
					}
					else{
						double score = lastPaperScoreList[paper] / paperSuccessorWeightSumList[paper];
						for(auto &neighbor: graph[paper].successorList){
							double vote = score * paperWeightList[neighbor];
							this -> paperScoreList[neighbor] += vote;
						}
					}
				}
				
				double scoreDifference = 0.0;
				for(int paper = 0; paper < paperCount; paper ++){
					double zeroVote = zeroOutdegreePaperScore * paperRegularizationList[paper];
					this -> paperScoreList[paper] = (1.0 - this -> dampingFactor) * paperRegularizationList[paper]
													+ this -> dampingFactor * (zeroVote + this -> paperScoreList[paper]);

					scoreDifference += fabs(lastPaperScoreList[paper] - this -> paperScoreList[paper]);
				}
				printf("\t\tScore difference = %f\n", scoreDifference);
				if(scoreDifference < this -> convergenceThreshold){
					printf("\t\tConvergence\n");
					break;
				}

				lastPaperScoreList = this -> paperScoreList;
			}
			
		}
		
	public:
		PageRank(double dampingFactor, double convergenceThreshold){
			this -> dampingFactor = dampingFactor;
			this -> convergenceThreshold = convergenceThreshold;
			this -> smallPositiveValue = 1e-10;
		}

		void run(vector< Paper > &graph){
			int paperCount = graph.size();
			this -> paperScoreList.assign(paperCount, 0.0);

			this -> runPaperPageRank(graph);
		}
		
		void printResults(char *fileName, vector< unsigned int > &paperNameList){
			int paperCount = this -> paperScoreList.size();
			
			double maxValue = 0, minValue = 0;
			bool isMaxFirst = true, isMinFirst = true;
			for(int paper = 0; paper < paperCount; paper ++){
				double value = this -> paperScoreList[paper];
				if(isMaxFirst || maxValue < value){
					maxValue = value;
					isMaxFirst = false;
				}
				if(isMinFirst || minValue > value){
					minValue = value;
					isMinFirst = false;
				}
			}
			
			FILE *outFile = fopen(fileName, "w");
			for(int paper = 0; paper < paperCount; paper ++){
				unsigned int name = paperNameList[paper];
				fprintf(outFile, "%08X\t%.15e\n", name, (this -> paperScoreList[paper] - minValue) / (maxValue - minValue));
			}	
			fclose(outFile);
		}
};

void readGraphFile(char *graphFileName, vector< Paper > &graph, unordered_map< unsigned int, int > &namePaperDict, vector < unsigned int > &paperNameList){
	FILE *inFile = fopen(graphFileName, "r");
	unsigned int nameFrom, nameTo;
	int paperIndex= 0;
	while(fscanf(inFile, "%X%X", &nameFrom, &nameTo) == 2){
		if(namePaperDict.find(nameFrom) == namePaperDict.end()){
			namePaperDict[nameFrom] = paperIndex;
			Paper newPaper;
			graph.push_back(newPaper);
			paperNameList.push_back(nameFrom);
			paperIndex += 1;
		}
		if(namePaperDict.find(nameTo) == namePaperDict.end()){
			namePaperDict[nameTo] = paperIndex;
			Paper newPaper;
			graph.push_back(newPaper);
			paperNameList.push_back(nameTo);
			paperIndex += 1;
		}
		
		int paperFrom = namePaperDict[nameFrom];
		int paperTo = namePaperDict[nameTo];
		graph[paperFrom].successorList.push_back(paperTo);
		graph[paperTo].predecessorList.push_back(paperFrom);
	}
	fclose(inFile);
}

void readFeatureFile(char *paperFileName, unordered_map< unsigned int, int > &namePaperDict, vector< Paper > &graph){
	unordered_map< unsigned int, int > nameConferenceDict, nameJournalDict, nameAuthorDict, nameAffiliationDict;
	int conferenceIndex = 0, journalIndex = 0, authorIndex = 0, affiliationIndex = 0;
	
	FILE *inFile = fopen(paperFileName, "r");
	char line[100000];
	while(fgets(line, 100000, inFile)){
		char *linePtr = line;
		int charCount;

		unsigned int paperName;
		sscanf(linePtr, "%X%n", &paperName, &charCount);
		linePtr += charCount;
		int paper = namePaperDict[paperName];

		int year;
		sscanf(linePtr, "%d%n", &year, &charCount);
		linePtr += charCount;
		graph[paper].year = year;

		int conferenceJournalExists;
		sscanf(linePtr, "%d%n", &conferenceJournalExists, &charCount);
		linePtr += charCount;
		
		switch(conferenceJournalExists){
			case 1:{
				unsigned int conferenceName;
				sscanf(linePtr, "%X%n", &conferenceName, &charCount);
				linePtr += charCount;

				if(nameConferenceDict.find(conferenceName) == nameConferenceDict.end()){
					nameConferenceDict[conferenceName] = conferenceIndex;
					conferenceIndex += 1;
				}

				int conference = nameConferenceDict[conferenceName];
				graph[paper].conference = conference;
				graph[paper].journal = -1;
				break;
			}
			case 2:{
				unsigned int journalName;
				sscanf(linePtr, "%X%n", &journalName, &charCount);
				linePtr += charCount;

				if(nameConferenceDict.find(journalName) == nameConferenceDict.end()){
					nameConferenceDict[journalName] = journalIndex;
					journalIndex += 1;
				}

				int journal = nameConferenceDict[journalName];
				graph[paper].journal = journal;
				graph[paper].conference = -1;
				break;
			}
			default:
				graph[paper].conference = -1;
				graph[paper].journal = -1;
		}

		int authorCount;
		sscanf(linePtr, "%d%n", &authorCount, &charCount);
		linePtr += charCount;

		for(int a = 0; a < authorCount; a ++){
			unsigned int authorName;
			sscanf(linePtr, "%X%n", &authorName, &charCount);
			linePtr += charCount;	
	
			if(nameAuthorDict.find(authorName) == nameAuthorDict.end()){
				nameAuthorDict[authorName] = authorIndex;
				authorIndex += 1;
			}

			int author = nameAuthorDict[authorName];
			graph[paper].authorList.push_back(author);
		}
		
		int affiliationCount;
		sscanf(linePtr, "%d%n", &affiliationCount, &charCount);
		linePtr += charCount;

		for(int a = 0; a < affiliationCount; a ++){
			unsigned int affiliationName;
			sscanf(linePtr, "%X%n", &affiliationName, &charCount);
			linePtr += charCount;

			if(nameAffiliationDict.find(affiliationName) == nameAffiliationDict.end()){
				nameAffiliationDict[affiliationName] = affiliationIndex;
				affiliationIndex += 1;
			}

			int affiliation = nameAffiliationDict[affiliationName];
			graph[paper].affiliationList.push_back(affiliation);
		}
	}
	fclose(inFile);
}

int main(int argc, char *argv[]){
	char *graphFileName = argv[1];
	char *paperFileName = argv[2];
	char *outputFileName = argv[3];

	double dampingFactor = 0.5;
	double convergenceThreshold = 1e-4;
	
	vector< Paper > graph;
	unordered_map< unsigned int, int > namePaperDict;
	vector < unsigned int > paperNameList;

	printf("Read graph file\n");
	readGraphFile(graphFileName, graph, namePaperDict, paperNameList);
	int paperCount = graph.size();
	printf("\tNumber of papers = %d\n", paperCount);
	
	printf("Read feature file\n");
	readFeatureFile(paperFileName, namePaperDict, graph);

	printf("Run PageRank algorithm\n");
	PageRank pagerank(dampingFactor, convergenceThreshold);
	pagerank.run(graph);

	printf("Print PageRank scores\n");
	pagerank.printResults(outputFileName, paperNameList);
	
	printf("OK\n");
	return 0;
}
