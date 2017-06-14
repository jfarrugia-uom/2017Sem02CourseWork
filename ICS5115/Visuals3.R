library("ggplot2")
library("plyr")

getwd()
heart <-read.table("./CourseWork/2017Sem02CourseWork/ICS5115/heart-rate.txt", header=TRUE) # heart-rate.txt
students <-read.csv("./CourseWork/2017Sem02CourseWork/ICS5115/students.csv") # students.csv
# get structure of object
str(students)
dim(students)
# 48 rows by 13 columns
nrow(students)
ncol(students)
summary(students)

siblings <- with(students,Brothers+Sisters)
#ggplot (data, aesthestics) + geometric representation layer

# to order by decreasing frequency
# first isolate students.Level into factor variable
level.factor <- factor(students$Level)
# play around with order and count (we need plyr)
# this for instance returns the levels in desired order as a character vector - not really useful
level.freq <- as.character(count(students, "Level")[order(count(students, "Level")$freq, decreasing=TRUE), 1])
str(level.freq)
# this returns the order of the Level column as an integer vector - this is much better
order(count(students, "Level")$freq, decreasing = TRUE)
# finally this resets the factor level order and assigns the result to the original column
students$Level <-
  factor(level.factor, levels(level.factor)[order(count(students, "Level")$freq, decreasing = TRUE)]  )
# now we can plot
ggplot(students, aes(x=Level)) + geom_bar()
# here we can confirm the re-ordered factor levels
levels(students$Level)



