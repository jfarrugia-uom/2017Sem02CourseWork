#install.packages("babynames")
library("babynames")

#summary(babynames)
some.names <- babynames[babynames$year == 1979 & babynames$prop >= 0.004, c("name", "prop", "sex")]$name
sort(some.names)

if (file.exists("~/host/friends.txt")) file.remove("~/host/friends.txt") 
# head of list is facebook user; rest are her list of friends

generate.friends <- function(x, y) {
  friends <- c(x, y)
  friend.count <- sample(5:10, 1) # friend count from 5 to 10
  friend.list <- sample(some.names, friend.count, replace = FALSE)
  friends <- c(friends, friend.list[ !(friend.list %in% friends[1:2]) ])
  cat(friends, file="~/host/friends.txt", append=TRUE, sep ="\t")
  cat("\n", file="~/host/friends.txt", append=TRUE)
  return(friends)
}

initial.list <- sample(some.names, 10, replace=FALSE)
cat(initial.list, file="~/host/friends.txt", append=TRUE, sep ="\t")
cat("\n", file="~/host/friends.txt", append=TRUE)
sapply(initial.list[2:10], generate.friends, initial.list[1])


