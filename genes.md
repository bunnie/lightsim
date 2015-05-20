Schema for light genes.
=======================

A light genome is nominally a diploid genome consisting of a set of characteristics that can be
"bred" among each other.

Some base rules of the genome properties
----------------------------------------

* All colors are in HSV
* All genes have a bit for "dominant" or "recessive"
* The phenotype of each gene is safe in its own right: you can't have a non-sense gene,
or a gene that crashes the system.
* Combinations of phenotypes can happen that don't necessarily make sense or produce an
undersireable effect.
* A set of gene is called an "indivdiual". All individuals are hermaphrodites, e.g.
can function as a role of male or female.

Think of genes as more like a set of sliders to effect modules, rather than as code
for effects themselves.

These are examples of logical genes:
* Periodicity of a sine wave
* Color gamut
* Accelerometer fall sensitivity threshold
* Accelerometer action (e.g, triggers strobe, dims, reverses pattern)
* Sound sensitivity threshold
* Sound action (e.g. what behavior happens in response to sound)
* Strobing frequency (if such a phenotype was to be displayed)

These are examples of genes that have no logic on their own right:
- Loop counters
- Snippets of executable code
- Entry points into existing code

In other words, genes set parameters on existing, well-vetted functions that can make a lighting
effect in its own right, and are not raw snippets of code that we hope can run and not crash
the embedded OS. The main motivation for this restriction is to limit the amount of testing required
to ensure that the system is stable in the context of a diverse set of user-settable parameters.

Gene encoding
-------------
Genes are encoded using gray code. This allows mutations to occur by flipping a random
bit without disturbing the net effect too much. Thus, mutation extent is referred to
in the sense of hamming distance.

Mutations
---------
Every time a child is born, there is a chance of introucing novel mutations. The mutation
rate is set by a global parameter, and is typically low but non-zero (e.g. about a 50% chance
probability on every breeding run that you'll have a mutation with hamming distance 1 one
the entire set of parameters).

Mutation rate can be increased by vigorously shaking the device prior to having sex. There
is a completion bar on the UI to indicate relative mutation rate, and the more the user
shakes, the higher the bar gets. Over time, the bar will start to empty as well. In its
maximal setting, the mutation rate should be 100% chance of mutation of about half the genes
with a hamming distance > 4.

Breeding effects
----------------

In addition to a set of fall-back non-breedable effects (e.g. calm pattern, saftey pattern,
wave rainbow pattern, etc.), there is a mode where the user can show their genetic effect.

The genetic effect is actually a "family" of effects. You have two parent individuals, and
two child individuals. In order to breed, one must promote one or both children into the role
of parents, vacating the child position (and deleting the parent), allowing for a new
child to enter the family. The effect that is shown is user selectable -- a user can show
any of the four members of the family as their default effect.

As all individuals are hermaphrodites, flow of genetic information during breeding is
two-way. In other words, two parents breeding with each other will each create a child
on the respective families. However, the mutations and dominant/recessive selectivities
are specific to each badge, so it's not guaranteed that two people breeding with each
other will create the exact same children.

Children are allowed to attempt to breed, but since a child occupies the child spot the
result will not create a new child on the children's side. Here are specific family
planning guidelines:

* Two parents with at least one child slot available on each side will result in one
new child on each side.
* A parent cannot breed with another parent if they do not have a child slot available
* A child breeding with a parent which has a child slot available will result in a new
child on the parent side, but the child side there is no change in state.
* Any individual can be deleted from any slot independently
* A child can be promoted to a parent, overwriting the parent
* One who deletes their entire family has left the breeding pool (there should be
a warning in the UI on this case)

