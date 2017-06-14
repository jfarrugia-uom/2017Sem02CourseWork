######## Kovalchik ATP tests ##############
summary(atp_elo)

atp_elo[atp_elo$tourney_start_date > as.Date('2016/1/1') & atp_elo$tourney_name == 'Australian Open', ][1:5, ]

tennis.pe.all[grepl(".*Australian.*2016.*Halys.*Dodig.*", tennis.pe.all$FULL_DESCRIPTION), ][1:5, ]

############ normalising player attempt ##############################
####### Load ATP/WTA Official Tennis Players #######
# load male ATP players
atp.players <- read.csv("atp_players.csv", header=TRUE, sep=",")
atp.players$dob <- as.Date(strptime(atp.players$dob, '%Y%m%d'))
atp.players$governing.body <- rep("ATP", nrow(atp.players))
# load women WTA players
wta.players <- read.csv("wta_players.csv", header=TRUE, sep=",")
wta.players$dob <- as.Date(strptime(wta.players$dob, '%Y%m%d'))
wta.players$governing.body <- rep("WTA", nrow(wta.players))
# fuse the two sets
all.official.players <- rbind(atp.players, wta.players)
# create player full name
all.official.players$full.name <- paste(all.official.players$name, all.official.players$surname, sep=" ")


# infer the contestants
tennis.contestants <-
  tennis.pe.all.year %>%
  distinct(EVENT_ID, year, WIN_FLAG, SELECTION) %>%
  spread(WIN_FLAG, SELECTION)

# set column names
colnames(tennis.contestants) <- c("EVENTID", "year", "loser", "winner")
# remove extra white spaces
tennis.contestants$loser <- trimws(gsub("\\s{2,}", " ", tennis.contestants$loser))
tennis.contestants$winner <- trimws(gsub("\\s{2,}", " ", tennis.contestants$winner))
# remove cases where we only have winner or loser meaning that we don't have all odds available 
tennis.contestants <-tennis.contestants[-which(is.na(tennis.contestants$loser) | is.na(tennis.contestants$winner) ), ]

# attempt to normalise player names - first get list of all contestants
all.players <- unique(union(tennis.contestants$winner, tennis.contestants$loser))
# ordered list of players
all.players[order(all.players)]
# set of all matched players - no lookup required
matched.all.players <- all.players[(all.players %in% all.official.players$full.name )]

# create lookup 
lookup.players <- data.frame(source = all.players[!(all.players %in% all.official.players$full.name)], stringsAsFactors = FALSE)
# add target field and initialise to NA
lookup.players$target <- NA
# match partial players represented as <n surname>; ex. r Federer
lookup.players[which(grepl("^[A-Za-z]{1} .+", lookup.players$source)), ]$target <-
  sapply(sub(" ", ".*", lookup.players$source[grepl("^[A-Za-z]{1} .+", lookup.players$source)]), 
         function(x) { matched.all.players[grepl(x, matched.all.players, ignore.case = TRUE)][1]}, USE.NAMES = FALSE)
# match partial players represented exclusively with surname
lookup.players[which(!grepl("\\s{1,}", lookup.players$source)), ]$target <-
  sapply(all.players[!grepl("\\s{1,}", all.players)], 
         function(x) { matched.all.players[grepl(x, matched.all.players, ignore.case = TRUE)][1]}, USE.NAMES = FALSE)

# add matched players to lookup frame
lookup.players <- rbind(lookup.players, data.frame(source=matched.all.players, target=matched.all.players))

# create data.frame from matched.players vector to use with fuzzy joint
matched.all.players.df <- as.data.frame(unique(matched.all.players))
colnames(matched.all.players.df) = c("matched.target")
# idea is to capture player names that are a distance of 1 from known matched targets and set lookup target accordingly
fuzzy.players <-
  lookup.players %>%
  filter(is.na(target)) %>%
  stringdist_inner_join(matched.all.players.df, by=c(source="matched.target"), max_dist=1, distance_col="distance") %>%
  mutate(target=matched.target) %>%
  select(source, target) %>%
  as.data.frame
# set fuzzy target to character
fuzzy.players$target <- as.character(fuzzy.players$target)

lookup.players[lookup.players$source %in% fuzzy.players$source,]$target <- 
  fuzzy.players[fuzzy.players$source %in% lookup.players$source,"target"]

# remove players we cannot normalise
lookup.nona <- na.omit(lookup.players)

# number of matches where I wasn't able to normalise at least one of the participants
# these matches will be ignored
length(
  union(
    tennis.contestants[!tennis.contestants$winner %in% lookup.nona$source, "EVENTID"],
    tennis.contestants[!tennis.contestants$loser %in% lookup.nona$source, "EVENTID"]
  )  
)

# get ATP players in our list that cannot be matched to our ELO data set
elo.names <-
  atp_elo.2008 %>%
  filter( as.numeric(format(tourney_start_date, "%Y")) >= 2008) %>%
  distinct(player_name) %>%
  arrange(player_name)

tmp.atp.players <-
  lookup.players %>%
  inner_join(all.official.players, by=c(target="full.name")) %>%
  filter(governing.body=="ATP") %>%
  select(target) %>%
  as.data.frame
# ok, only retired players are returned who haveprobably played a senior tournament for which wagers were placed
tmp.atp.players[!tmp.atp.players$target %in% elo.names$player_name,]  

# this should return the senior guys plus all the women who will be found in the wta_elo set
matched.all.players[!matched.all.players %in% elo.names$player_name][
  order(matched.all.players[!matched.all.players %in% elo.names$player_name])]

tennis.contestants.norm <- 
  tennis.contestants %>%
  inner_join(lookup.nona, by=c(loser="source")) %>%
  inner_join(lookup.nona, by=c(winner="source")) %>%
  select(EVENTID, year, loser=target.x, winner=target.y) %>%
  as.data.frame

nrow(tennis.contestants.norm)
nrow(tennis.contestants)