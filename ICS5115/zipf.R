library(plyr)
library(data.table)
library(ggplot2)
#install.packages("data.table", dependencies = TRUE)
getwd()
setwd("/home/jfarrugia/CourseWork/2017Sem02CourseWork/ICS5115")
# list Hadoop reducer files 
temp <- list.files(pattern = "part*")
# load in dataframe
##hadoop.orig = apply(temp, read.delim(header=FALSE))
hadoop.orig <- ldply(temp, read.delim, header=FALSE, sep="\t")

str(hadoop.orig)
summary(hadoop.orig)
# rename colnames 
colnames(hadoop.orig) <- c("word", "frequency")
# find rows where freq is greater than 50000
hadoop.orig[hadoop.orig$frequency>50000, ]
# check data.frame dimensions
dim(hadoop.orig)
# count of original rows
nrow(hadoop.orig)
# count of deduplicated rows (2 ways)
nrow(count(hadoop.orig, c("word")))
length(unique(hadoop.orig$word))

# aggregate using table method 
hadoop.table <- data.table(hadoop.orig)
hadoop.table.uniq <- hadoop.table[, .(frequency.Sum = sum(frequency)), by=word]

hadoop.table[word=="divinity"]
hadoop.table.uniq[word=="divinity"]
# aggregate in dataframe
hadoop.df.uniq <- aggregate(x = hadoop.orig$frequency, 
                      by = list(word = hadoop.orig$word), 
                      FUN = sum)
help(aggregate)
nrow(hadoop.df.uniq)
summary(hadoop.df.uniq)
hadoop.df.uniq[hadoop.df.uniq$word=="divinity",]

hadoop.df.rank <-
transform(hadoop.df.uniq, 
          freq.rank = ave(x,
                          FUN = function(x) rank(-x, ties.method = "first")))
# rename columns again because aggregate renamed frequency column to x
colnames(hadoop.df.rank) <- c("word", "frequency", "rank")
summary(hadoop.df.rank)
head(hadoop.df.rank[order(hadoop.df.rank$rank), ], 10)
hadoop.df.rank <- hadoop.df.rank[order(hadoop.df.rank$rank), ]
nrow(hadoop.df.rank)
total.occurrences <- sum(hadoop.df.rank$frequency)

ggplot(hadoop.df.rank, aes(x=rank, y=frequency)) + geom_line() + 
  scale_x_continuous(trans='log10') + 
  scale_y_continuous(trans='log10') +
  labs(x="Log Rank",y="Log Frequency")
+ #geom_smooth(method='lm'),formula=y~x)


  eq = function(x){1/x}
ggplot(data.frame(x=c(1, 50)), 
       aes(x=x)) + stat_function(fun=eq, geom="line") + xlab("x") + ylab("y") +
  scale_x_continuous(trans='log10') + 
  scale_y_continuous(trans='log10') 

harmonic <- c(1:33929)
ref.harmonic.vector <- (1/c(1:33929))/ sum(1/harmonic)

plot(ecdf(ref.harmonic.vector))
plot(ecdf(hadoop.df.rank$frequency/total.occurrences))

head(ref.harmonic.vector)
head((hadoop.df.rank$frequency)/total.occurrences)

ks.test(ref.harmonic.vector, hadoop.df.rank$frequency/total.occurrences)