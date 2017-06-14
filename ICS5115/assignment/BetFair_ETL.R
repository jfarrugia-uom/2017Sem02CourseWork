library(plyr)
library(dplyr)
library(fuzzyjoin)
library(tidyr)
library(tools)
library(lubridate)

setwd("/home/jfarrugia/CourseWork/2017Sem02CourseWork/ICS5115/assignment")

clean.lines <- function(x) {
  tmp1 <- gsub("\",\"", "\"|\"", x)
  tmp1 <- gsub("\"", "", tmp1)
  tmp1 <- noquote(tmp1)
  ifelse(grepl("^SPORTS", tmp1), tmp1, 
         (ifelse(grepl("^2\\|.+PE$", tmp1), tmp1, NA)))
}

clean.and.filter <- function(betfair.data.file, parent.archive) {
  unzip(parent.archive, betfair.data.file, exdir = "./stage")
  cat("=====> Loading", betfair.data.file, "\n")
  write.to.file <- paste("stage", paste("tennis_", betfair.data.file, sep=""), sep="/")
  
  if (!file.exists(write.to.file)) {
    raw.csv.lines <- readLines(paste("stage", betfair.data.file, sep = "/"));      
    clean.csv.lines <- sapply(raw.csv.lines, clean.lines, USE.NAMES = FALSE)
    write.csv(clean.csv.lines[!is.na(clean.csv.lines)], file=write.to.file, quote=FALSE, row.names = FALSE)  
  }
  unlink(paste("stage", betfair.data.file, sep = "/" ))  
}

# perform two-step unarchiving;
# 1) unravel top-level zip;
# 2)  For every compressed file in archive 
# 2a) uncompress in staging area
# 2b) clean, filter tennis, pre-event odds only
# 2c) save contents in separate pipe-separated file
# 3) delete uncompressed file
unzip.and.extract <- function(a.file, parent.archive) {
  cat("Doing", a.file, "from", parent.archive, "\n")
  # unzip "archive in archive"
  unzip(parent.archive, a.file, exdir="./stage")
  # unzip csv file from unzipped archive
  if (!grepl("\\.rar", a.file)) {
    # get content details
    archive.contents <- unzip(paste("stage", a.file, sep="/"), list=TRUE)
    nested_archive <- archive.contents[grepl("\\.zip|\\.rar", archive.contents$Name), ]$Name
    # recursively call function to deal with archive within archive
    sapply(nested_archive, unzip.and.extract, paste("stage", a.file, sep="/"))
    # now deal with actual content
    text.contents <- archive.contents[grepl("\\.csv|\\.text", archive.contents$Name), ]$Name
    # apply clean.and.filter function on every content file
    sapply(text.contents, clean.and.filter, paste("stage", a.file, sep="/") )
    # remove uncompressed file not to clutter stage directory
    unlink(paste("stage", a.file, sep="/"))
  }
}

check.if.exists = function(tennis.file) {
  if (grepl("\\.rar", tennis.file)) {
    tennis.file <- paste("stage", tennis.file, sep="/")
    if (!file.exists(tennis.file)) {
      cat(tennis.file, "NOT found!", "\n")
    }
  } else {
    tennis.file2 <- gsub("\\.zip", ".csv", tennis.file)
    tennis.file2 <- paste("tennis_", tennis.file2, sep="")
    tennis.file2 <- paste("stage", tennis.file2, sep="/")
    if (!file.exists(tennis.file2)) {
      cat(tennis.file2, "NOT found!", "\n")
    }
  }
}

# get list of all zip files to consider
all.zip.files <- list.files(".", pattern="*.zip")
# iterate over all files 
for (i in all.zip.files[1:10]) {
  cat("Doing", i, "\n")
  archive.list <- unzip(i, list=TRUE)
  #sapply(archive.list$Name, unzip.and.extract, i)
  sapply(archive.list$Name, check.if.exists)
}


# 
##### We now have a bunch of tennis files containing PE odds for a variety of events #####
#
# based on observation, we know 
all.tennis.files <- list.files("./stage", pattern = "(tennis_other.*)|(tennis_bfinf_other.*).csv")
all.tennis.files <- paste("stage", all.tennis.files, sep='/')

#nrows <- sapply( all.tennis.files, function(f) nrow(read.csv(f, skip=1, header=TRUE, sep="|")) )

load.file.bulk <- function(tennis.file) {
  tennis.df <- read.csv(tennis.file, skip = 1, header=TRUE, sep='|', stringsAsFactors = FALSE)  
  tennis.df <- tennis.df[tennis.df$EVENT == "Match Odds" & tennis.df$NUMBER_BETS >= 20 , ]

}

# create data.frame from all match odds pre-event data
tennis.pe.all <- ldply(all.tennis.files, load.file.bulk)
tennis.pe.all.bak <- tennis.pe.all
#tennis.pe.all <- tennis.pe.all.bak
# check rows extracted
nrow(tennis.pe.all) #roughly 540,000
# summary of data.frame
summary(tennis.pe.all)

# retain entries where description starts with "Group A".  Descriptions not starting with "Group A" represents older data
tennis.pe.all <- filter(tennis.pe.all, startsWith(FULL_DESCRIPTION, "Group A"))


# change date strings to POSIX timestamps
for (col in c("SETTLED_DATE", "DT.ACTUAL_OFF", "LATEST_TAKEN", "FIRST_TAKEN","ACTUAL_OFF")) {
  tennis.pe.all[[col]] <- as.POSIXct(strptime(tennis.pe.all[[col]], '%d-%m-%Y %H:%M:%S'))
}
# SCHEDULED_OFF does not feature seconds
tennis.pe.all$SCHEDULED_OFF <- as.POSIXct(strptime(tennis.pe.all$SCHEDULED_OFF, '%d-%m-%Y %H:%M'))
# confirm change of type

# add year to tennis.pe.all data.frame
tennis.pe.all.year <- mutate(tennis.pe.all, year=format(SCHEDULED_OFF, "%Y"))

desc.lengths <- sapply(tennis.pe.all.year$FULL_DESCRIPTION, function(x) { length(unlist(strsplit(x, "/"))) }, USE.NAMES = FALSE)
unique(desc.lengths)
token.6.or.more <- tennis.pe.all.year[which(desc.lengths >=  6), c("FULL_DESCRIPTION")]
length(which(grepl("doubles", token.6.or.more, ignore.case = TRUE)==TRUE))


# empirically, I observed that splitting description by "/" separator, and counting tokens:
# If token count > 5, 7/10 of the set will represent doubles games which I want to eliminate from my set
nrow(tennis.pe.all.year)

tennis.pe.all.year <- tennis.pe.all.year[-which(desc.lengths > 5),]

# 1) Add Tournament, round, player and opponent
# 2) For every row: 
# 2a)   Add odds that have attracted most betting;
# 2b)   Add average betted amount (Will include laid amounts too);
# 2c)   Add difference between current (this row) odds and last posted odds

extract.tournament <- function (description) {
  split.details <- unlist(strsplit(description, "/"))
  tourney <- trimws(gsub("[0-9]+", "", split.details[2]))
}

extract.round <- function(description) {
  split.details <- unlist(strsplit(description, "/"))
  round.1 <- ifelse(length(split.details) == 3, "Unknown", 
                    ifelse(length(split.details) == 4, split.details[3], split.details[4])             
  )
  trimws(gsub("Matches", "", round.1))
}

extract.player <- function(description, index) {
  split.details <- unlist(strsplit(description, "/"))
  # the player pair usually found in the last token of the description field
  player.pair <- split.details[length(split.details)]
  trimws(gsub("[0-9.]", "", unlist(strsplit(player.pair, " v "))[index]) )
}

# since we're joing with ELO tennis match history, we don't need these details
tennis.pe.all.year$tourney <- sapply(tennis.pe.all.year$FULL_DESCRIPTION, extract.tournament, USE.NAMES = FALSE)
tennis.pe.all.year$round <- sapply(tennis.pe.all.year$FULL_DESCRIPTION, extract.round, USE.NAMES = FALSE)
tennis.pe.all.year$player <- sapply(tennis.pe.all.year$FULL_DESCRIPTION, extract.player, 1, USE.NAMES =  FALSE)
tennis.pe.all.year$opponent <- sapply(tennis.pe.all.year$FULL_DESCRIPTION, extract.player, 2, USE.NAMES =  FALSE)

# player and opponent will be pulled by pivoting SELECTION on event and year
# tourney, round, governing.body will be extracted from ELO history

summary(tennis.pe.all.year)
# convert selection to Title Case
tennis.pe.all.year$SELECTION <- toTitleCase(tolower(tennis.pe.all.year$SELECTION))

write.table(tennis.pe.all.year, file="tennis_match_odds.csv", sep = "|", row.names = FALSE)
# in case of crash load tennsis semi-cleaned data from here
tennis.pe.all.year <- read.csv(file="tennis_match_odds.csv", header=TRUE, sep="|", stringsAsFactors = FALSE)


# filter out games that took place prior to 2008
atp_elo.2008 <-
  atp_elo %>%
  filter(as.numeric(format(atp_elo$tourney_start_date, "%Y")) >= 2008) %>%
  as.data.frame


strip.surname <- function(x){
  full.name <- unlist(strsplit(x, " "));
  full.name[length(full.name)]
}

# add columns containing player and opponent surnames respectively.  Will be used to join on BetFair data
# two players can encounter each other often but we shall limit the result to rows where difference between
# tourney start date and scheduled off date is at its narrowest
atp_elo.2008$player_surname <- sapply(atp_elo.2008$player_name, strip.surname)
atp_elo.2008$opponent_surname <- sapply(atp_elo.2008$opponent_name, strip.surname)

# aggregate by event and year; get schedule date for every event
tennis.event.dates <-
  tennis.pe.all.year %>%
  group_by(year, EVENT_ID) %>%
  summarize(event_schedule_date = min(SCHEDULED_OFF) ) %>%
  as.data.frame %>%
  mutate(event_schedule_date = as.Date(event_schedule_date)) 


# strip surname from player and opponent, set column names to lower case
tennis.tidy <-
  tennis.pe.all.year %>%
  #distinct(EVENT_ID, year, player, opponent, SELECTION) %>%
  mutate(player = sapply(player, strip.surname), opponent = sapply(opponent, strip.surname)) %>%
  select(year, event_id = EVENT_ID, player, opponent,
         scheduled_off = SCHEDULED_OFF, actual_off = DT.ACTUAL_OFF,
         selection = SELECTION, odds = ODDS, number_bets = NUMBER_BETS, volume_matched = VOLUME_MATCHED,
         is_win = WIN_FLAG, 
         first_taken = FIRST_TAKEN, latest_taken = LATEST_TAKEN, betfair_tourney = tourney
  )


# normalise selection on the basis of player and opponent surname
tennis.tidy$selection <- 
  mapply(function(x,y,z) {
    ifelse(grepl(y, x, ignore.case = TRUE), y, z)
  }, tennis.tidy$selection, tennis.tidy$player, tennis.tidy$opponent, USE.NAMES = FALSE)

# get condensed list, leaving out odd combinations
tennis.matches <-
  tennis.tidy %>%
  distinct(year, event_id, player, opponent) %>%
  as.data.frame

# embellish with event schedule date
tennis.matches <-
  tennis.matches %>%
  inner_join(tennis.event.dates, by=c(event_id="EVENT_ID","year"="year")) %>%
  select(year, event_id, player, opponent, event_schedule_date) %>%
  as.data.frame

# now get the ELO information from the ATP ELO history
# same can be used to combine WTA ELO predictions
atp.matches.elo <- 
  tennis.matches %>%
  inner_join(atp_elo.2008, by=c(player="player_surname", opponent="opponent_surname")) %>%
  filter(event_schedule_date >= tourney_start_date) %>%
  mutate(days_elapsed = as.numeric(difftime(event_schedule_date, tourney_start_date,units="days"))) %>%
  group_by(year, event_id) %>%
  mutate(min_days_elapsed = min(days_elapsed), opponent_prediction = 1- prediction) %>%
  filter(days_elapsed == min_days_elapsed, days_elapsed < 100) %>%
  select(year, event_id, player, opponent, event_schedule_date, player_rank, opponent_rank, 
         player_age, opponent_age, tourney_name, tourney_level, round, win, prediction, opponent_prediction
  ) %>%
  as.data.frame 

# possible duplicates even though event_id is different
duplicate.remove <-
  atp.matches.elo %>%
  group_by(player, opponent, event_schedule_date) %>%  
  select(event_id, player, opponent, event_schedule_date, prediction) %>%
  mutate(max_event_id=max(event_id), min_prediction = min(prediction), cnt=n_distinct(event_id)) %>%
  filter(cnt>1, event_id==max_event_id)

atp.matches.elo <- atp.matches.elo[!atp.matches.elo$event_id %in% duplicate.remove$event_id,]

duplicate.remove.2 <-
  atp.matches.elo %>%
  group_by(player, opponent, event_schedule_date) %>%  
  select(event_id, player, opponent, event_schedule_date, prediction) %>%
  mutate(max_event_id=max(event_id), min_prediction = min(prediction), cnt=n()) %>%
  filter(cnt>1, prediction==min_prediction)


atp.matches.elo <- atp.matches.elo[!atp.matches.elo$event_id %in% duplicate.remove.2$event_id,] 

# join ELO prediction with Betfair data.  Now I have a model on which to base value bets
atp.final.data <-
  tennis.tidy %>%
  inner_join(atp.matches.elo, by=c(year="year", event_id="event_id")) %>%
  mutate(imputed_probability = 1/odds) %>%
  select(year, event_id, tourney_name, tourney_level, round,
         player=player.x, opponent=opponent.x,
         scheduled_off, actual_off, selection, odds, imputed_probability, number_bets, volume_matched,
         selection_win = is_win, player_win = win,
         first_taken, latest_taken, player_rank, opponent_rank, player_age, opponent_age, prediction, opponent_prediction) %>%
  as.data.frame

# calculate weighted odds 
weighted.odds <- 
  tennis.tidy %>%
  select(year, event_id, player, opponent, selection, is_win, odds, number_bets, volume_matched) %>%
  group_by(year, event_id, is_win) %>%  
  summarise(max_volume = max(volume_matched), tot_odds_offered = n(), 
         sum_volume = sum(volume_matched), sum_odds = sum(odds * volume_matched)) %>%
  mutate(weighted_odds = round(sum_odds / sum_volume, 2)) %>%
  select(year, event_id, is_win, weighted_odds)
  distinct(year, event_id, is_win, weighted_odds)
  as.data.frame

  
# remove reference to ATP ELO data
rm(tennis.pe.all)
rm(atp_elo)

# get reduced data frame containing 1 row by event and outcome including total volume matched, total bets placed and
# weighted odds
totals <- 
  tennis.tidy %>%
  group_by(year, event_id, player, opponent, selection, is_win) %>%
  summarise(total_volume=sum(volume_matched), total_bets=sum(number_bets)) %>%
  inner_join(weighted.odds, by=c(year="year", event_id="event_id", is_win="is_win")) %>%
  select(year, event_id, player, opponent, selection, is_win, total_volume, total_bets, weighted_odds) %>%
  as.data.frame

atp.elo.totals <-  
  totals %>% 
      inner_join(atp.matches.elo, by=c(year="year", event_id="event_id")) %>%
      #filter(event_id==21086700) %>%
      mutate(imputed_probability=1/weighted_odds, 
             selection_elo_prediction=ifelse(selection==player.x, prediction, opponent_prediction)) %>%
      select(year, event_id, player=player.x, opponent=opponent.x, event_schedule_date, 
             player_age, opponent_age, tourney_name, tourney_level, round,
             selection, is_win, total_volume, total_bets, weighted_odds,
             imputed_probability, selection_elo_prediction) %>%
      as.data.frame

atp.elo.totals$tourney_name <- toTitleCase(tolower(atp.elo.totals$tourney_name))