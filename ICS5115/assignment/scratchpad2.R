tennis.contestants %>%
  filter(year==2017) 
  

tennis.pe.all.year %>%
  filter(year==2017) %>%
  summarize(max_date = max(SCHEDULED_OFF))


1-(atp_elo %>%
  filter(player_name=='Tomas Berdych', opponent_name=='Jo Wilfried Tsonga' ))[12,"prediction"]

max(atp_elo[as.numeric(format(atp_elo$tourney_start_date, "%Y")) == 2017, ]$tourney_start_date)

atp_elo %>%
  filter(grepl("Rotterdam", tourney_name ), as.numeric(format(atp_elo$tourney_start_date, "%Y")) == 2016)

all.official.players %>%
  filter(grepl("Mecir", full.name))

colnames(atp_elo)
colnames(tennis.pe.all.year)
colnames(tennis.contestants)
summary(tennis.pe.all.year)

  
lookup.nona <- na.omit(lookup.players)
lookup.nona[is.na(lookup.nona$target), ]
nrow(lookup.nona)


unique(atp_elo$tourney_name)
unique(tennis.pe.all.year$tourney)[order(unique(tennis.pe.all.year$tourney))]

colnames(atp_elo)
atp_elo.2008 %>%
  filter(as.numeric(format(tourney_start_date, "%Y")) >= 2008) %>%
  distinct(tourney_name, tourney_level) %>%
  filter(tourney_level %in% c("Grand Slams", "Masters", "Tour Finals", "250 or 500")) 

atp_elo.2008 %>%
  filter(tourney_name=="New Haven") %>%
  slice(1:2)

tennis.pe.all.year %>%
  filter(grepl("", tourney, ignore.case=TRUE)) %>%
  slice(1:10)

atp_elo.2008 <-
  atp_elo %>%
  filter(as.numeric(format(atp_elo$tourney_start_date, "%Y")) >= 2008) %>%
  as.data.frame

atp_elo.2008 %>%
  slice(1:10)
      
tennis.contestants %>%
  filter(winner=="Dudi Sela", loser=="Donald Young")

tennis.pe.all.year %>%
  filter(EVENT_ID==21149237)


tennis.contestants.norm %>%
  inner_joint(tennis.event.dates)
  
  slice(1:10) %>%
  group_by(year, EVENT_ID) %>%
  summarize(cnt = n()) %>%
  filter(cnt > 1)


year(as.Date("2016/1/14"))


summary(tennis.contestants.norm)
nrow(tennis.contestants.norm)
nrow(tennis.event.dates)
tennis.event.dates[!tennis.event.dates$EVENT_ID %in% tennis.contestants.norm$EVENTID,][1:10, ]

tennis.pe.all.year[tennis.pe.all.year$EVENT_ID==100365388,]


##################################################################################################################
colnames(tennis.pe.all.year)
colnames(tennis.contestants.norm)
colnames(tennis.event.dates)
colnames(foo.1.summary)
colnames(foo.1)
colnames(atp.foo.3)
summary(atp_elo.2008)
typeof(atp_elo.2008$tourney_start_date)
summary(tennis.event.dates)
typeof(tennis.event.dates$event_schedule_date)

strip.surname <- function(x){
  full.name <- unlist(strsplit(x, " "));
  full.name[length(full.name)]
}

# strip surname from player and opponent, set column names to lower case
foo.1 <-
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
foo.1$selection <- 
  mapply(function(x,y,z) {
    ifelse(grepl(y, x), y, z)
  }, foo.1$selection, foo.1$player, foo.1$opponent, USE.NAMES = FALSE)

# get condensed list, leaving out odd combinations
foo.1.summary <-
foo.1 %>%
  distinct(year, event_id, player, opponent) %>%
  as.data.frame

# embellish with event schedule date
foo.1.summary <-
  foo.1.summary %>%
    inner_join(tennis.event.dates, by=c(event_id="EVENT_ID","year"="year")) %>%
    select(year, event_id, player, opponent, event_schedule_date) %>%
    as.data.frame

# now get the ELO information from the ATP ELO history
atp.foo.3 <- 
  foo.1.summary %>%
  inner_join(atp_elo.2008, by=c(player="player_surname", opponent="opponent_surname")) %>%
  filter(event_schedule_date >= tourney_start_date) %>%
  mutate(days_elapsed = as.numeric(difftime(event_schedule_date, tourney_start_date,units="days"))) %>%
  group_by(year, event_id) %>%
  mutate(min_days_elapsed = min(days_elapsed), opponent_prediction = 1- prediction) %>%
  filter(days_elapsed == min_days_elapsed, days_elapsed <= 365 ) %>%
  select(year, event_id, player, opponent, event_schedule_date, player_rank, opponent_rank, 
         player_age, opponent_age, tourney_name, tourney_level, round, win, prediction, opponent_prediction, days_elapsed
         ) %>%
  as.data.frame 
  
# join ELO prediction with Betfair data.  Now I have a model on which to base value bets
atp.final.data <- 
foo.1 %>% 
  inner_join(atp.foo.3, by=c(year="year", event_id="event_id")) %>%
  mutate(imputed_probability = 1/odds) %>%
  select(year, event_id, tourney_name, tourney_level, round,
         player=player.x, opponent=opponent.x, 
         scheduled_off, actual_off, selection, odds, imputed_probability, number_bets, volume_matched, is_win, win,
         first_taken, latest_taken, player_rank, opponent_rank, player_age, opponent_age, prediction, opponent_prediction) %>%
  as.data.frame

foo.1 %>%
  filter(event_id==122733415)

nrow(foo.1)
nrow(foo.1.summary)
nrow(atp.foo.3)
nrow(atp.final.data)
summary(atp.foo.3)
nrow(tennis.event.dates)

foo.1 %>%
  filter(event_id == 126813765) 126815489

atp.foo.3 %>%
  filter(prediction > 0.5) %>%
  group_by(win) %>%
  summarise(n())

atp.foo.3 %>%
  group_by(win) %>%
  summarise(n())

atp.final.data %>%
  filter(selection_win != player_win) %>%
  slice(1:2)

duplicate.remove <-
atp.foo.3 %>%
    group_by(player, opponent, event_schedule_date) %>%  
    select(event_id, player, opponent, event_schedule_date, prediction) %>%
    mutate(max_event_id=max(event_id), min_prediction = min(prediction), cnt=n_distinct(event_id)) %>%
    filter(cnt>1, event_id==max_event_id)

duplicate.remove.2 <-
atp.foo.3 %>%
  group_by(player, opponent, event_schedule_date) %>%  
  select(event_id, player, opponent, event_schedule_date, prediction) %>%
  mutate(max_event_id=max(event_id), min_prediction = min(prediction), cnt=n()) %>%
  filter(cnt>1, prediction==min_prediction)

atp.foo.3 <- atp.foo.3[!atp.foo.3$event_id %in% duplicate.remove$event_id,]
atp.foo.3 <- atp.foo.3[!atp.foo.3$event_id %in% duplicate.remove.2$event_id,] 
nrow(atp.foo.3)

atp.foo.3 %>%
  filter(player=="Traver", opponent=="Martin")
boxplot(atp.foo.3$days_elapsed[atp.foo.3$days_elapsed <= 30])

nrow(
atp.foo.3 %>%
  filter(days_elapsed >= 365)
)

atp.foo.3 %>%
  filter(event_id == 21314988)

tennis.pe.all.year %>%
  filter(EVENT_ID %in% (c(125402864, 125402946)))

foo.1.summary %>%
  filter(player=="Baghdatis", opponent=="Nadal")

foo.1.summary %>%
  filter(event_id==21314988)

atp_elo.2008 %>%
  filter(player_surname=="Djokovic", opponent_surname=="Davydenko",tourney_start_date<="2008/11/11")

atp.final.data %>%
  filter(player==opponent)

atp.final.data %>%
  group_by(year, player_win) %>%
  filter(prediction > 0.55, selection==player, prediction-imputed_probability >= 0.05, selection_win==1 ) %>%
  summarise(cnt=n_distinct(event_id)) %>%
  arrange(desc(year), player_win)


atp.final.data %>%
  group_by(opponent, year) %>%
  filter(opponent_prediction > 0.8, selection==opponent, selection_win==0) %>%
  summarise(cnt=n_distinct(event_id)) %>%
  filter(cnt>2) %>%
  arrange(desc(year),desc(cnt))
  